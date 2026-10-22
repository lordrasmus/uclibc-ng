#include "complex_impl.h"

/* clog.c in double; there is no long double version of it.  */
long double complex clogl(long double complex z)
{
	return clog(z);
}
