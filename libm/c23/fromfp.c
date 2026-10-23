/* fromfp, ufromfp, fromfpx, ufromfpx (ISO C23): round to an integer in
   the given direction and check that it fits in WIDTH bits, signed or
   unsigned.  The x forms also raise inexact when the value changed.

   Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.  */

#include "c23_private.h"

static double fromfp_common(double x, int rnd, unsigned int width,
			    int is_unsigned, int raise_inexact)
{
	double r, lim;

	if (width == 0 || !isfinite(x))
		return c23_invalid();
	switch (rnd) {
	case FP_INT_UPWARD:
		r = ceil(x);
		break;
	case FP_INT_DOWNWARD:
		r = floor(x);
		break;
	case FP_INT_TOWARDZERO:
		r = trunc(x);
		break;
	case FP_INT_TONEARESTFROMZERO:
		r = round(x);
		break;
	case FP_INT_TONEAREST:
		r = roundeven(x);
		break;
	default:			/* unspecified; glibc truncates */
		r = trunc(x);
		break;
	}
	/* 2^width (or 2^(width-1)); past the double range every finite
	   value fits.  */
	if (is_unsigned) {
		lim = width < 1100 ? ldexp(1.0, width) : HUGE_VAL;
		if (r < 0 || r >= lim)
			return c23_invalid();
	} else {
		lim = width < 1100 ? ldexp(1.0, width - 1) : HUGE_VAL;
		if (r < -lim || r >= lim)
			return c23_invalid();
	}
	if (raise_inexact && r != x) {
		volatile double one = 1.0;

		one += 0x1p-60;
	}
	return r;
}

double fromfp(double x, int rnd, unsigned int width)
{
	return fromfp_common(x, rnd, width, 0, 0);
}

double ufromfp(double x, int rnd, unsigned int width)
{
	return fromfp_common(x, rnd, width, 1, 0);
}

double fromfpx(double x, int rnd, unsigned int width)
{
	return fromfp_common(x, rnd, width, 0, 1);
}

double ufromfpx(double x, int rnd, unsigned int width)
{
	return fromfp_common(x, rnd, width, 1, 1);
}

float fromfpf(float x, int rnd, unsigned int width)
{
	return fromfp_common(x, rnd, width, 0, 0);
}

float ufromfpf(float x, int rnd, unsigned int width)
{
	return fromfp_common(x, rnd, width, 1, 0);
}

float fromfpxf(float x, int rnd, unsigned int width)
{
	return fromfp_common(x, rnd, width, 0, 1);
}

float ufromfpxf(float x, int rnd, unsigned int width)
{
	return fromfp_common(x, rnd, width, 1, 1);
}

#if C23_LDBL
long double fromfpl(long double x, int rnd, unsigned int width)
{
	return fromfp_common(x, rnd, width, 0, 0);
}

long double ufromfpl(long double x, int rnd, unsigned int width)
{
	return fromfp_common(x, rnd, width, 1, 0);
}

long double fromfpxl(long double x, int rnd, unsigned int width)
{
	return fromfp_common(x, rnd, width, 0, 1);
}

long double ufromfpxl(long double x, int rnd, unsigned int width)
{
	return fromfp_common(x, rnd, width, 1, 1);
}
#endif
