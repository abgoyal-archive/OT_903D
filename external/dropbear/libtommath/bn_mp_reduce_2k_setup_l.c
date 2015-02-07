#include <tommath.h>
#ifdef BN_MP_REDUCE_2K_SETUP_L_C

/* determines the setup value */
int mp_reduce_2k_setup_l(mp_int *a, mp_int *d)
{
   int    res;
   mp_int tmp;
   
   if ((res = mp_init(&tmp)) != MP_OKAY) {
      return res;
   }
   
   if ((res = mp_2expt(&tmp, mp_count_bits(a))) != MP_OKAY) {
      goto ERR;
   }
   
   if ((res = s_mp_sub(&tmp, a, d)) != MP_OKAY) {
      goto ERR;
   }
   
ERR:
   mp_clear(&tmp);
   return res;
}
#endif

/* $Source: /cvs/libtom/libtommath/bn_mp_reduce_2k_setup_l.c,v $ */
/* $Revision: 1.3 $ */
/* $Date: 2006/03/31 14:18:44 $ */
