#include "complex_impl.h"

/* sin(z) = -i sinh(i z) */

double complex csin(double complex z)
{
	z = csinh(CMPLX(-cimag(z), creal(z)));
	return CMPLX(cimag(z), -creal(z));
}
libm_hidden_def(csin)
