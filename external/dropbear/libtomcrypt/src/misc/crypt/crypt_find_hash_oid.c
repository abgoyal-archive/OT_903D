#include "tomcrypt.h"


int find_hash_oid(const unsigned long *ID, unsigned long IDlen)
{
   int x;
   LTC_ARGCHK(ID != NULL);
   LTC_MUTEX_LOCK(&ltc_hash_mutex);
   for (x = 0; x < TAB_SIZE; x++) {
       if (hash_descriptor[x].name != NULL && hash_descriptor[x].OIDlen == IDlen && !XMEMCMP(hash_descriptor[x].OID, ID, sizeof(unsigned long) * IDlen)) {
          LTC_MUTEX_UNLOCK(&ltc_hash_mutex);
          return x;
       }
   }
   LTC_MUTEX_UNLOCK(&ltc_hash_mutex);
   return -1;
}

/* $Source: /cvs/libtom/libtomcrypt/src/misc/crypt/crypt_find_hash_oid.c,v $ */
/* $Revision: 1.4 $ */
/* $Date: 2006/11/01 09:28:17 $ */
