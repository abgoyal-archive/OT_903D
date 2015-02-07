
/* @(#)e_acosh.c 1.3 95/01/18 */


#include "fdlibm.h"

#ifdef __STDC__
static const double 
#else
static double 
#endif
one	= 1.0,
ln2	= 6.93147180559945286227e-01;  /* 0x3FE62E42, 0xFEFA39EF */

#ifdef __STDC__
	double __ieee754_acosh(double x)
#else
	double __ieee754_acosh(x)
	double x;
#endif
{	
	double t;
	int hx;
	hx = __HI(x);
	if(hx<0x3ff00000) {		/* x < 1 */
	    return (x-x)/(x-x);
	} else if(hx >=0x41b00000) {	/* x > 2**28 */
	    if(hx >=0x7ff00000) {	/* x is inf of NaN */
	        return x+x;
	    } else 
		return __ieee754_log(x)+ln2;	/* acosh(huge)=ieee_log(2x) */
	} else if(((hx-0x3ff00000)|__LO(x))==0) {
	    return 0.0;			/* acosh(1) = 0 */
	} else if (hx > 0x40000000) {	/* 2**28 > x > 2 */
	    t=x*x;
	    return __ieee754_log(2.0*x-one/(x+ieee_sqrt(t-one)));
	} else {			/* 1<x<2 */
	    t = x-one;
	    return ieee_log1p(t+ieee_sqrt(2.0*t+t*t));
	}
}
