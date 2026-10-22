#include "complex_impl.h"

/* catrig.c in double; there is no long double version of it.  */
long double complex casinhl(long double complex z)
{
	return casinh(z);
}
