//
// nonintel.c: code for non-Intel processors only
//

#include "quakedef.h"
// #include "r_local.h"
// #include "d_local.h"

#if	!id386

void R_Surf8Patch ()
{
	// we only patch code on Intel
}


void R_Surf16Patch ()
{
	// we only patch code on Intel
}


void R_SurfacePatch (void)
{
	// we only patch code on Intel
}


#endif	// !id386

