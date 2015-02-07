#include "tomcrypt.h"


#ifdef LTC_ECB_MODE

int ecb_done(symmetric_ECB *ecb)
{
   int err;
   LTC_ARGCHK(ecb != NULL);

   if ((err = cipher_is_valid(ecb->cipher)) != CRYPT_OK) {
      return err;
   }
   cipher_descriptor[ecb->cipher].done(&ecb->key);
   return CRYPT_OK;
}

   

#endif

/* $Source: /cvs/libtom/libtomcrypt/src/modes/ecb/ecb_done.c,v $ */
/* $Revision: 1.7 $ */
/* $Date: 2006/06/29 01:51:34 $ */
