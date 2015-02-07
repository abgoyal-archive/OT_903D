#include "tomcrypt.h"



#ifdef LTC_DER

int der_decode_boolean(const unsigned char *in, unsigned long inlen,
                                       int *out)
{
   LTC_ARGCHK(in  != NULL);
   LTC_ARGCHK(out != NULL);
   
   if (inlen != 3 || in[0] != 0x01 || in[1] != 0x01 || (in[2] != 0x00 && in[2] != 0xFF)) {
      return CRYPT_INVALID_ARG;
   }
   
   *out = (in[2]==0xFF) ? 1 : 0;
   
   return CRYPT_OK;
}

#endif

/* $Source: /cvs/libtom/libtomcrypt/src/pk/asn1/der/boolean/der_decode_boolean.c,v $ */
/* $Revision: 1.1 $ */
/* $Date: 2006/04/22 17:01:59 $ */
