#include "tomcrypt.h"


#ifdef LTC_PMAC

void pmac_shift_xor(pmac_state *pmac)
{
   int x, y;
   y = pmac_ntz(pmac->block_index++);
#ifdef LTC_FAST
   for (x = 0; x < pmac->block_len; x += sizeof(LTC_FAST_TYPE)) {
       *((LTC_FAST_TYPE*)((unsigned char *)pmac->Li + x)) ^=
       *((LTC_FAST_TYPE*)((unsigned char *)pmac->Ls[y] + x));
   }
#else
   for (x = 0; x < pmac->block_len; x++) {
       pmac->Li[x] ^= pmac->Ls[y][x];
   }
#endif
}

#endif

/* $Source: /cvs/libtom/libtomcrypt/src/mac/pmac/pmac_shift_xor.c,v $ */
/* $Revision: 1.6 $ */
/* $Date: 2006/11/03 00:39:49 $ */
