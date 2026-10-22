#include "complex_impl.h"

/* catrig.c in double; there is no long double version of it.  */
long double complex casinl(long double complex z)
{
	return casin(z);
}
