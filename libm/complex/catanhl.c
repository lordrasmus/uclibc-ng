#include "complex_impl.h"

/* catrig.c in double; there is no long double version of it.  */
long double complex catanhl(long double complex z)
{
	return catanh(z);
}
