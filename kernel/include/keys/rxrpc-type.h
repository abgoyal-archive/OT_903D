

#ifndef _KEYS_RXRPC_TYPE_H
#define _KEYS_RXRPC_TYPE_H

#include <linux/key.h>

extern struct key_type key_type_rxrpc;

extern struct key *rxrpc_get_null_key(const char *);

struct rxkad_key {
	u32	vice_id;
	u32	start;			/* time at which ticket starts */
	u32	expiry;			/* time at which ticket expires */
	u32	kvno;			/* key version number */
	u8	primary_flag;		/* T if key for primary cell for this user */
	u16	ticket_len;		/* length of ticket[] */
	u8	session_key[8];		/* DES session key */
	u8	ticket[0];		/* the encrypted ticket */
};

struct krb5_principal {
	u8	n_name_parts;		/* N of parts of the name part of the principal */
	char	**name_parts;		/* parts of the name part of the principal */
	char	*realm;			/* parts of the realm part of the principal */
};

struct krb5_tagged_data {
	/* for tag value, see /usr/include/krb5/krb5.h
	 * - KRB5_AUTHDATA_* for auth data
	 * - 
	 */
	s32		tag;
	u32		data_len;
	u8		*data;
};

struct rxk5_key {
	u64			authtime;	/* time at which auth token generated */
	u64			starttime;	/* time at which auth token starts */
	u64			endtime;	/* time at which auth token expired */
	u64			renew_till;	/* time to which auth token can be renewed */
	s32			is_skey;	/* T if ticket is encrypted in another ticket's
						 * skey */
	s32			flags;		/* mask of TKT_FLG_* bits (krb5/krb5.h) */
	struct krb5_principal	client;		/* client principal name */
	struct krb5_principal	server;		/* server principal name */
	u16			ticket_len;	/* length of ticket */
	u16			ticket2_len;	/* length of second ticket */
	u8			n_authdata;	/* number of authorisation data elements */
	u8			n_addresses;	/* number of addresses */
	struct krb5_tagged_data	session;	/* session data; tag is enctype */
	struct krb5_tagged_data *addresses;	/* addresses */
	u8			*ticket;	/* krb5 ticket */
	u8			*ticket2;	/* second krb5 ticket, if related to ticket (via
						 * DUPLICATE-SKEY or ENC-TKT-IN-SKEY) */
	struct krb5_tagged_data *authdata;	/* authorisation data */
};

struct rxrpc_key_token {
	u16	security_index;		/* RxRPC header security index */
	struct rxrpc_key_token *next;	/* the next token in the list */
	union {
		struct rxkad_key *kad;
		struct rxk5_key *k5;
	};
};

struct rxrpc_key_data_v1 {
	u32		kif_version;		/* 1 */
	u16		security_index;
	u16		ticket_length;
	u32		expiry;			/* time_t */
	u32		kvno;
	u8		session_key[8];
	u8		ticket[0];
};

#define AFSTOKEN_LENGTH_MAX		16384	/* max payload size */
#define AFSTOKEN_STRING_MAX		256	/* max small string length */
#define AFSTOKEN_DATA_MAX		64	/* max small data length */
#define AFSTOKEN_CELL_MAX		64	/* max cellname length */
#define AFSTOKEN_MAX			8	/* max tokens per payload */
#define AFSTOKEN_BDATALN_MAX		16384	/* max big data length */
#define AFSTOKEN_RK_TIX_MAX		12000	/* max RxKAD ticket size */
#define AFSTOKEN_GK_KEY_MAX		64	/* max GSSAPI key size */
#define AFSTOKEN_GK_TOKEN_MAX		16384	/* max GSSAPI token size */
#define AFSTOKEN_K5_COMPONENTS_MAX	16	/* max K5 components */
#define AFSTOKEN_K5_NAME_MAX		128	/* max K5 name length */
#define AFSTOKEN_K5_REALM_MAX		64	/* max K5 realm name length */
#define AFSTOKEN_K5_TIX_MAX		16384	/* max K5 ticket size */
#define AFSTOKEN_K5_ADDRESSES_MAX	16	/* max K5 addresses */
#define AFSTOKEN_K5_AUTHDATA_MAX	16	/* max K5 pieces of auth data */

#endif /* _KEYS_RXRPC_TYPE_H */
