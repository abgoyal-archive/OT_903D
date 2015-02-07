
/* @(#)w_sinh.c 1.3 95/01/18 */


#include "fdlibm.h"

#ifdef __STDC__
	double ieee_sinh(double x)		/* wrapper sinh */
#else
	double ieee_sinh(x)			/* wrapper sinh */
	double x;
#endif
{
#ifdef _IEEE_LIBM
	return __ieee754_sinh(x);
#else
	double z; 
	z = __ieee754_sinh(x);
	if(_LIB_VERSION == _IEEE_) return z;
	if(!ieee_finite(z)&&ieee_finite(x)) {
	    return __kernel_standard(x,x,25); /* sinh overflow */
	} else
	    return z;
#endif
}
