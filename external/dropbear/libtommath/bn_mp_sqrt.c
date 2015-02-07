#include <tommath.h>
#ifdef BN_MP_SQRT_C

/* this function is less generic than mp_n_root, simpler and faster */
int mp_sqrt(mp_int *arg, mp_int *ret) 
{
  int res;
  mp_int t1,t2;

  /* must be positive */
  if (arg->sign == MP_NEG) {
    return MP_VAL;
  }

  /* easy out */
  if (mp_iszero(arg) == MP_YES) {
    mp_zero(ret);
    return MP_OKAY;
  }

  if ((res = mp_init_copy(&t1, arg)) != MP_OKAY) {
    return res;
  }

  if ((res = mp_init(&t2)) != MP_OKAY) {
    goto E2;
  }

  /* First approx. (not very bad for large arg) */
  mp_rshd (&t1,t1.used/2);

  /* t1 > 0  */ 
  if ((res = mp_div(arg,&t1,&t2,NULL)) != MP_OKAY) {
    goto E1;
  }
  if ((res = mp_add(&t1,&t2,&t1)) != MP_OKAY) {
    goto E1;
  }
  if ((res = mp_div_2(&t1,&t1)) != MP_OKAY) {
    goto E1;
  }
  /* And now t1 > sqrt(arg) */
  do { 
    if ((res = mp_div(arg,&t1,&t2,NULL)) != MP_OKAY) {
      goto E1;
    }
    if ((res = mp_add(&t1,&t2,&t1)) != MP_OKAY) {
      goto E1;
    }
    if ((res = mp_div_2(&t1,&t1)) != MP_OKAY) {
      goto E1;
    }
    /* t1 >= sqrt(arg) >= t2 at this point */
  } while (mp_cmp_mag(&t1,&t2) == MP_GT);

  mp_exch(&t1,ret);

E1: mp_clear(&t2);
E2: mp_clear(&t1);
  return res;
}

#endif

/* $Source: /cvs/libtom/libtommath/bn_mp_sqrt.c,v $ */
/* $Revision: 1.3 $ */
/* $Date: 2006/03/31 14:18:44 $ */
