
#ifndef _ALGO_H_

#define _ALGO_H_

#include "includes.h"
#include "buffer.h"

struct Algo_Type {

	unsigned char *name; /* identifying name */
	char val; /* a value for this cipher, or -1 for invalid */
	void *data; /* algorithm specific data */
	unsigned usable : 1; /* whether we can use this algorithm */

};

typedef struct Algo_Type algo_type;

/* lists mapping ssh types of algorithms to internal values */
extern algo_type sshkex[];
extern algo_type sshhostkey[];
extern algo_type sshciphers[];
extern algo_type sshhashes[];
extern algo_type sshcompress[];

extern const struct dropbear_cipher dropbear_nocipher;
extern const struct dropbear_hash dropbear_nohash;

struct dropbear_cipher {
	const struct ltc_cipher_descriptor *cipherdesc;
	unsigned long keysize;
	unsigned char blocksize;
};

struct dropbear_hash {
	const struct ltc_hash_descriptor *hashdesc;
	unsigned long keysize;
	unsigned char hashsize;
};

void crypto_init();
int have_algo(char* algo, size_t algolen, algo_type algos[]);
void buf_put_algolist(buffer * buf, algo_type localalgos[]);

algo_type * svr_buf_match_algo(buffer* buf, algo_type localalgos[],
		int *goodguess);
algo_type * cli_buf_match_algo(buffer* buf, algo_type localalgos[],
		int *goodguess);

#endif /* _ALGO_H_ */
