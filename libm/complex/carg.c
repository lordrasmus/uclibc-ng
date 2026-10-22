#include "complex_impl.h"

double carg(double complex z)
{
	return atan2(cimag(z), creal(z));
}
libm_hidden_def(carg)
