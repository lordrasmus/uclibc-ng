#include "complex_impl.h"

/* catrig.c in double; there is no long double version of it.  */
long double complex cacosl(long double complex z)
{
	return cacos(z);
}
