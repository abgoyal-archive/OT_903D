#include <tommath.h>
#ifdef BN_MP_CLEAR_C

/* clear one (frees)  */
void
mp_clear (mp_int * a)
{
  volatile mp_digit *p;
  int len;

  /* only do anything if a hasn't been freed previously */
  if (a->dp != NULL) {
    /* first zero the digits */
	len = a->alloc;
	p = a->dp;
	while (len--) {
		*p++ = 0;
	}

    /* free ram */
    XFREE(a->dp);

    /* reset members to make debugging easier */
    a->dp    = NULL;
    a->alloc = a->used = 0;
    a->sign  = MP_ZPOS;
  }
}
#endif

/* $Source: /cvs/libtom/libtommath/bn_mp_clear.c,v $ */
/* $Revision: 1.3 $ */
/* $Date: 2006/03/31 14:18:44 $ */
