

/* $Id: addrinfo.h,v 1.4 2002/06/11 17:13:36 itojun Exp $ */

#ifndef HAVE_ADDRINFO

#define	EAI_ADDRFAMILY	 1	/* address family for hostname not supported */
#define	EAI_AGAIN	 2	/* temporary failure in name resolution */
#define	EAI_BADFLAGS	 3	/* invalid value for ai_flags */
#define	EAI_FAIL	 4	/* non-recoverable failure in name resolution */
#define	EAI_FAMILY	 5	/* ai_family not supported */
#define	EAI_MEMORY	 6	/* memory allocation failure */
#define	EAI_NODATA	 7	/* no address associated with hostname */
#define	EAI_NONAME	 8	/* hostname nor servname provided, or not known */
#define	EAI_SERVICE	 9	/* servname not supported for ai_socktype */
#define	EAI_SOCKTYPE	10	/* ai_socktype not supported */
#define	EAI_SYSTEM	11	/* system error returned in errno */
#define EAI_BADHINTS	12
#define EAI_PROTOCOL	13
#define EAI_MAX		14

/* internal error */
#define	NETDB_INTERNAL	-1	/* see errno */

#define	AI_PASSIVE	0x00000001 /* get address to use bind() */
#define	AI_CANONNAME	0x00000002 /* fill ai_canonname */
#define	AI_NUMERICHOST	0x00000004 /* prevent name resolution */
/* valid flags for addrinfo */
#define	AI_MASK		(AI_PASSIVE | AI_CANONNAME | AI_NUMERICHOST)

#define	AI_ALL		0x00000100 /* IPv6 and IPv4-mapped (with AI_V4MAPPED) */
#define	AI_V4MAPPED_CFG	0x00000200 /* accept IPv4-mapped if kernel supports */
#define	AI_ADDRCONFIG	0x00000400 /* only if any address is assigned */
#define	AI_V4MAPPED	0x00000800 /* accept IPv4-mapped IPv6 address */
/* special recommended flags for getipnodebyname */
#define	AI_DEFAULT	(AI_V4MAPPED_CFG | AI_ADDRCONFIG)

struct addrinfo {
	int	ai_flags;	/* AI_PASSIVE, AI_CANONNAME */
	int	ai_family;	/* PF_xxx */
	int	ai_socktype;	/* SOCK_xxx */
	int	ai_protocol;	/* 0 or IPPROTO_xxx for IPv4 and IPv6 */
	size_t	ai_addrlen;	/* length of ai_addr */
	char	*ai_canonname;	/* canonical name for hostname */
	struct sockaddr *ai_addr;	/* binary address */
	struct addrinfo *ai_next;	/* next structure in linked list */
};

extern void freeaddrinfo (struct addrinfo *);
extern void freehostent (struct hostent *);
extern char *gai_strerror (int);
extern int getaddrinfo (const char *, const char *,
			    const struct addrinfo *, struct addrinfo **);
extern int getnameinfo (const struct sockaddr *, size_t, char *,
			    size_t, char *, size_t, int);
extern struct hostent *getipnodebyaddr (const void *, size_t, int, int *);
extern struct hostent *getipnodebyname (const char *, int, int, int *);
extern int inet_pton (int, const char *, void *);
extern const char *inet_ntop (int, const void *, char *, size_t);
#endif /* HAVE_ADDRINFO */

#ifndef NI_MAXHOST
#define	NI_MAXHOST	1025
#endif
#ifndef NI_MAXSERV
#define	NI_MAXSERV	32
#endif

#ifndef NI_NOFQDN
#define	NI_NOFQDN	0x00000001
#endif
#ifndef NI_NUMERICHOST
#define	NI_NUMERICHOST	0x00000002
#endif
#ifndef NI_NAMEREQD
#define	NI_NAMEREQD	0x00000004
#endif
#ifndef NI_NUMERICSERV
#define	NI_NUMERICSERV	0x00000008
#endif
#ifndef NI_DGRAM
#define	NI_DGRAM	0x00000010
#endif
