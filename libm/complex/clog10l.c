#include "complex_impl.h"

/* clog10.c in double, like clogl.  */
long double complex clog10l(long double complex z)
{
	return clog10(z);
}
strong_alias(clog10l, __clog10l)
