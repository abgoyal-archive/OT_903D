#include "tomcrypt.h"


#ifdef MKAT

int katja_decrypt_key(const unsigned char *in,       unsigned long  inlen,
                          unsigned char *out,      unsigned long *outlen, 
                    const unsigned char *lparam,   unsigned long  lparamlen,
                          int            hash_idx, int           *stat,
                          katja_key       *key)
{
  unsigned long modulus_bitlen, modulus_bytelen, x;
  int           err;
  unsigned char *tmp;
  
  LTC_ARGCHK(out    != NULL);
  LTC_ARGCHK(outlen != NULL);
  LTC_ARGCHK(key    != NULL);
  LTC_ARGCHK(stat   != NULL);

  /* default to invalid */
  *stat = 0;

  /* valid hash ? */
  if ((err = hash_is_valid(hash_idx)) != CRYPT_OK) {
     return err;
  }
  
  /* get modulus len in bits */
  modulus_bitlen = mp_count_bits( (key->N));

  /* payload is upto pq, so we know q is 1/3rd the size of N and therefore pq is 2/3th the size */
 modulus_bitlen = ((modulus_bitlen << 1) / 3);

  /* round down to next byte */
  modulus_bitlen -= (modulus_bitlen & 7) + 8;

  /* outlen must be at least the size of the modulus */
  modulus_bytelen = mp_unsigned_bin_size( (key->N));
  if (modulus_bytelen != inlen) {
     return CRYPT_INVALID_PACKET;
  }

  /* allocate ram */
  tmp = XMALLOC(inlen);
  if (tmp == NULL) {
     return CRYPT_MEM;
  }

  /* rsa decode the packet */
  x = inlen;
  if ((err = katja_exptmod(in, inlen, tmp, &x, PK_PRIVATE, key)) != CRYPT_OK) {
     XFREE(tmp);
     return err;
  }

  /* shift right by modulus_bytelen - modulus_bitlen/8  bytes */
  for (x = 0; x < (modulus_bitlen >> 3); x++) {
     tmp[x] = tmp[x+(modulus_bytelen-(modulus_bitlen>>3))];
  }

  /* now OAEP decode the packet */
  err = pkcs_1_oaep_decode(tmp, x, lparam, lparamlen, modulus_bitlen, hash_idx,
                           out, outlen, stat);

  XFREE(tmp);
  return err;
}

#endif /* MRSA */





/* $Source: /cvs/libtom/libtomcrypt/src/pk/katja/katja_decrypt_key.c,v $ */
/* $Revision: 1.4 $ */
/* $Date: 2006/03/31 14:15:35 $ */
