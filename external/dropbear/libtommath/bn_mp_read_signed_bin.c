#include <tommath.h>
#ifdef BN_MP_READ_SIGNED_BIN_C

/* read signed bin, big endian, first byte is 0==positive or 1==negative */
int mp_read_signed_bin (mp_int * a, const unsigned char *b, int c)
{
  int     res;

  /* read magnitude */
  if ((res = mp_read_unsigned_bin (a, b + 1, c - 1)) != MP_OKAY) {
    return res;
  }

  /* first byte is 0 for positive, non-zero for negative */
  if (b[0] == 0) {
     a->sign = MP_ZPOS;
  } else {
     a->sign = MP_NEG;
  }

  return MP_OKAY;
}
#endif

/* $Source: /cvs/libtom/libtommath/bn_mp_read_signed_bin.c,v $ */
/* $Revision: 1.4 $ */
/* $Date: 2006/03/31 14:18:44 $ */
