
#include "tomcrypt.h"

#ifdef GCM_MODE

int gcm_memory(      int           cipher,
               const unsigned char *key,    unsigned long keylen,
               const unsigned char *IV,     unsigned long IVlen,
               const unsigned char *adata,  unsigned long adatalen,
                     unsigned char *pt,     unsigned long ptlen,
                     unsigned char *ct, 
                     unsigned char *tag,    unsigned long *taglen,
                               int direction)
{
    void      *orig;
    gcm_state *gcm;
    int        err;

    if ((err = cipher_is_valid(cipher)) != CRYPT_OK) {
       return err;
    }
 
    if (cipher_descriptor[cipher].accel_gcm_memory != NULL) {
       return 
         cipher_descriptor[cipher].accel_gcm_memory
                                          (key,   keylen,
                                           IV,    IVlen,
                                           adata, adatalen,
                                           pt,    ptlen,
                                           ct,
                                           tag,   taglen,
                                           direction);
    }



#ifndef GCM_TABLES_SSE2
    orig = gcm = XMALLOC(sizeof(*gcm));
#else
    orig = gcm = XMALLOC(sizeof(*gcm) + 16);
#endif
    if (gcm == NULL) {
        return CRYPT_MEM;
    }

   /* Force GCM to be on a multiple of 16 so we can use 128-bit aligned operations
    * note that we only modify gcm and keep orig intact.  This code is not portable
    * but again it's only for SSE2 anyways, so who cares?
    */
#ifdef GCM_TABLES_SSE2
   if ((unsigned long)gcm & 15) {
      gcm = (gcm_state *)((unsigned long)gcm + (16 - ((unsigned long)gcm & 15)));
   }
#endif

    if ((err = gcm_init(gcm, cipher, key, keylen)) != CRYPT_OK) {
       goto LTC_ERR;
    }
    if ((err = gcm_add_iv(gcm, IV, IVlen)) != CRYPT_OK) {
       goto LTC_ERR;
    }
    if ((err = gcm_add_aad(gcm, adata, adatalen)) != CRYPT_OK) {
       goto LTC_ERR;
    }
    if ((err = gcm_process(gcm, pt, ptlen, ct, direction)) != CRYPT_OK) {
       goto LTC_ERR;
    }
    err = gcm_done(gcm, tag, taglen);
LTC_ERR:
    XFREE(orig);
    return err;
}
#endif


/* $Source: /cvs/libtom/libtomcrypt/src/encauth/gcm/gcm_memory.c,v $ */
/* $Revision: 1.23 $ */
/* $Date: 2006/09/07 10:00:57 $ */
