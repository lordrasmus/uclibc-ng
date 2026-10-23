/* roundeven (ISO C23): round to the nearest integer, ties to even,
   whatever the current rounding mode.

   Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.  */

#include "c23_private.h"

/* round() takes ties away from zero; for a tie, r - x is exactly 0.5
   (both are multiples of ulp(x)), and halving gives the even one.  */
double roundeven(double x)
{
	double r;

	if (!isfinite(x))
		return x + x;
	r = round(x);
	if (fabs(r - x) == 0.5)
		r = 2.0 * round(x * 0.5);
	return r;
}

float roundevenf(float x)
{
	float r;

	if (!isfinite(x))
		return x + x;
	r = roundf(x);
	if (fabsf(r - x) == 0.5f)
		r = 2.0f * roundf(x * 0.5f);
	return r;
}

#if C23_LDBL
long double roundevenl(long double x)
{
	return roundeven((double) x);
}
#endif
