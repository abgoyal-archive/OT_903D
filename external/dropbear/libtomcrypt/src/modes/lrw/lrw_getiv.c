#include "tomcrypt.h"


#ifdef LTC_LRW_MODE

int lrw_getiv(unsigned char *IV, unsigned long *len, symmetric_LRW *lrw)
{
   LTC_ARGCHK(IV != NULL);
   LTC_ARGCHK(len != NULL);
   LTC_ARGCHK(lrw != NULL);
   if (*len < 16) {
       *len = 16;
       return CRYPT_BUFFER_OVERFLOW;
   }

   XMEMCPY(IV, lrw->IV, 16);
   *len = 16;
   return CRYPT_OK;
}

#endif
/* $Source: /cvs/libtom/libtomcrypt/src/modes/lrw/lrw_getiv.c,v $ */
/* $Revision: 1.9 $ */
/* $Date: 2006/06/29 01:53:13 $ */
