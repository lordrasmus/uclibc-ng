/* nextup and nextdown (ISO C23): the next representable value up or
   down; unlike nextafter they raise no overflow or underflow.

   Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.  */

#include "c23_private.h"

double nextup(double x)
{
	uint64_t i = c23_asuint64(x);

	if (isnan(x))
		return x + x;
	if (i == 0x7ff0000000000000ULL)		/* +inf */
		return x;
	if (x == 0)
		return 0x1p-1074;
	i += (i >> 63) ? -1 : 1;		/* the magnitude goes down if negative */
	return c23_asdouble(i);
}

double nextdown(double x)
{
	return -nextup(-x);
}

float nextupf(float x)
{
	uint32_t i = c23_asuint32(x);

	if (isnan(x))
		return x + x;
	if (i == 0x7f800000U)
		return x;
	if (x == 0)
		return 0x1p-149f;
	i += (i >> 31) ? -1 : 1;
	return c23_asfloat(i);
}

float nextdownf(float x)
{
	return -nextupf(-x);
}

#if C23_LDBL
long double nextupl(long double x)
{
	return nextup((double) x);
}

long double nextdownl(long double x)
{
	return nextdown((double) x);
}
#endif
