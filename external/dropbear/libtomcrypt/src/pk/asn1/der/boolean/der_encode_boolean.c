#include "tomcrypt.h"



#ifdef LTC_DER

int der_encode_boolean(int in, 
                       unsigned char *out, unsigned long *outlen)
{
   LTC_ARGCHK(outlen != NULL);
   LTC_ARGCHK(out    != NULL);
   
   if (*outlen < 3) {
       *outlen = 3;
       return CRYPT_BUFFER_OVERFLOW;
   }
   
   *outlen = 3;
   out[0] = 0x01;
   out[1] = 0x01;
   out[2] = in ? 0xFF : 0x00;
   
   return CRYPT_OK;
}

#endif

/* $Source: /cvs/libtom/libtomcrypt/src/pk/asn1/der/boolean/der_encode_boolean.c,v $ */
/* $Revision: 1.3 $ */
/* $Date: 2006/06/16 21:53:41 $ */
