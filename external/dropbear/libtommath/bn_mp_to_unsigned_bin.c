#include <tommath.h>
#ifdef BN_MP_TO_UNSIGNED_BIN_C

/* store in unsigned [big endian] format */
int mp_to_unsigned_bin (mp_int * a, unsigned char *b)
{
  int     x, res;
  mp_int  t;

  if ((res = mp_init_copy (&t, a)) != MP_OKAY) {
    return res;
  }

  x = 0;
  while (mp_iszero (&t) == 0) {
#ifndef MP_8BIT
      b[x++] = (unsigned char) (t.dp[0] & 255);
#else
      b[x++] = (unsigned char) (t.dp[0] | ((t.dp[1] & 0x01) << 7));
#endif
    if ((res = mp_div_2d (&t, 8, &t, NULL)) != MP_OKAY) {
      mp_clear (&t);
      return res;
    }
  }
  bn_reverse (b, x);
  mp_clear (&t);
  return MP_OKAY;
}
#endif

/* $Source: /cvs/libtom/libtommath/bn_mp_to_unsigned_bin.c,v $ */
/* $Revision: 1.3 $ */
/* $Date: 2006/03/31 14:18:44 $ */
