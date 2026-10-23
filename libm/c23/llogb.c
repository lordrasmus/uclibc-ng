/* llogb (ISO C23): ilogb with a long result.

   Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.  */

#include "c23_private.h"
#include <limits.h>

long int llogb(double x)
{
	if (isnan(x))
		return FP_LLOGBNAN;
	if (x == 0)
		return FP_LLOGB0;
	if (isinf(x))
		return LONG_MAX;
	return ilogb(x);
}

long int llogbf(float x)
{
	return llogb(x);
}

#if C23_LDBL
long int llogbl(long double x)
{
	return llogb((double) x);
}
#endif
