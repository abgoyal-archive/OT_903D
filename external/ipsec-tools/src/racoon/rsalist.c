/*	$NetBSD: rsalist.c,v 1.4 2006/09/09 16:22:10 manu Exp $	*/

/* Id: rsalist.c,v 1.3 2004/11/08 12:04:23 ludvigm Exp */


#include "config.h"

#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/queue.h>
#include <sys/socket.h>
#include <netdb.h>

#include <openssl/bn.h>
#include <openssl/rsa.h>

#include "misc.h"
#include "plog.h"
#include "sockmisc.h"
#include "rsalist.h"
#include "genlist.h"
#include "remoteconf.h"
#include "crypto_openssl.h"

#ifndef LIST_FIRST
#define LIST_FIRST(head)        ((head)->lh_first)
#endif

#ifndef LIST_NEXT
#define LIST_NEXT(elm, field)   ((elm)->field.le_next)
#endif

/* from prsa_tok.l */
int prsa_parse_file(struct genlist *list, const char *fname, enum rsa_key_type type);

int
rsa_key_insert(struct genlist *list, struct netaddr *src,
	       struct netaddr *dst, RSA *rsa)
{
	struct rsa_key *rsa_key;

	rsa_key = calloc(sizeof(struct rsa_key), 1);
	rsa_key->rsa = rsa;

	if (src)
		rsa_key->src = src;
	else
		rsa_key->src = calloc(sizeof(*rsa_key->src), 1);

	if (dst)
		rsa_key->dst = dst;
	else
		rsa_key->dst = calloc(sizeof(*rsa_key->dst), 1);

	genlist_append(list, rsa_key);

	return 0;
}

static void *
rsa_key_dump_one(void *entry, void *arg)
{
	struct rsa_key *key = entry;

	plog(LLV_DEBUG, LOCATION, NULL, "Entry %s\n",
	     naddrwop2str_fromto("%s -> %s", key->src,
				 key->dst));
	if (loglevel > LLV_DEBUG)
		RSA_print_fp(stdout, key->rsa, 4);

	return NULL;
}

void
rsa_key_dump(struct genlist *list)
{
	genlist_foreach(list, rsa_key_dump_one, NULL);
}

static void *
rsa_list_count_one(void *entry, void *arg)
{
	if (arg)
		(*(unsigned long *)arg)++;
	return NULL;
}

unsigned long
rsa_list_count(struct genlist *list)
{
	unsigned long count = 0;
	genlist_foreach(list, rsa_list_count_one, &count);
	return count;
}

struct lookup_result {
	struct ph1handle *iph1;
	int max_score;
	struct genlist *winners;
};
	
static void *
rsa_lookup_key_one(void *entry, void *data)
{
	int local_score, remote_score;
	struct lookup_result *req = data;
	struct rsa_key *key = entry;

	local_score = naddr_score(key->src, req->iph1->local);
	remote_score = naddr_score(key->dst, req->iph1->remote);

	plog(LLV_DEBUG, LOCATION, NULL, "Entry %s scored %d/%d\n",
		naddrwop2str_fromto("%s -> %s", key->src, key->dst),
		local_score, remote_score);

	if (local_score >= 0 && remote_score >= 0) {
		if (local_score + remote_score > req->max_score) {
			req->max_score = local_score + remote_score;
//			genlist_free(req->winners, NULL);
		}

		if (local_score + remote_score >= req->max_score) {
			genlist_append(req->winners, key);
		}
	}

	/* Always traverse the whole list */
	return NULL;
}

struct genlist *
rsa_lookup_keys(struct ph1handle *iph1, int my)
{
	struct genlist *list;
	struct lookup_result r;

	plog(LLV_DEBUG, LOCATION, NULL, "Looking up RSA key for %s\n",
	     saddr2str_fromto("%s <-> %s", iph1->local, iph1->remote));

	r.iph1 = iph1;
	r.max_score = -1;
	r.winners = genlist_init();

	if (my)
		list = iph1->rmconf->rsa_private;
	else
		list = iph1->rmconf->rsa_public;

	genlist_foreach(list, rsa_lookup_key_one, &r);

	if (loglevel >= LLV_DEBUG)
		rsa_key_dump(r.winners);

	return r.winners;
}

int
rsa_parse_file(struct genlist *list, const char *fname, enum rsa_key_type type)
{
	int ret;
	
	plog(LLV_DEBUG, LOCATION, NULL, "Parsing %s\n", fname);
	ret = prsa_parse_file(list, fname, type);
	if (loglevel >= LLV_DEBUG)
		rsa_key_dump(list);
	return ret;
}

RSA *
rsa_try_check_rsasign(vchar_t *source, vchar_t *sig, struct genlist *list)
{
	struct rsa_key *key;
	struct genlist_entry *gp;

	for(key = genlist_next(list, &gp); key; key = genlist_next(NULL, &gp)) {
		plog(LLV_DEBUG, LOCATION, NULL, "Checking key %s...\n",
			naddrwop2str_fromto("%s -> %s", key->src, key->dst));
		if (eay_check_rsasign(source, sig, key->rsa) == 0) {
			plog(LLV_DEBUG, LOCATION, NULL, " ... YEAH!\n");
			return key->rsa;
		}
		plog(LLV_DEBUG, LOCATION, NULL, " ... nope.\n");
	}
	return NULL;
}
