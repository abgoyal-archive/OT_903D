#include <tommath.h>
#ifdef BN_MP_ZERO_C

/* set to zero */
void mp_zero (mp_int * a)
{
  int       n;
  mp_digit *tmp;

  a->sign = MP_ZPOS;
  a->used = 0;

  tmp = a->dp;
  for (n = 0; n < a->alloc; n++) {
     *tmp++ = 0;
  }
}
#endif

/* $Source: /cvs/libtom/libtommath/bn_mp_zero.c,v $ */
/* $Revision: 1.3 $ */
/* $Date: 2006/03/31 14:18:44 $ */
