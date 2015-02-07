#include "tomcrypt.h"


#ifdef LTC_F8_MODE

int f8_getiv(unsigned char *IV, unsigned long *len, symmetric_F8 *f8)
{
   LTC_ARGCHK(IV  != NULL);
   LTC_ARGCHK(len != NULL);
   LTC_ARGCHK(f8  != NULL);
   if ((unsigned long)f8->blocklen > *len) {
      *len = f8->blocklen;
      return CRYPT_BUFFER_OVERFLOW;
   }
   XMEMCPY(IV, f8->IV, f8->blocklen);
   *len = f8->blocklen;

   return CRYPT_OK;
}

#endif

/* $Source: /cvs/libtom/libtomcrypt/src/modes/f8/f8_getiv.c,v $ */
/* $Revision: 1.2 $ */
/* $Date: 2006/06/16 22:49:25 $ */
