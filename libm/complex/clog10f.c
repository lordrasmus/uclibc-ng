#include "complex_impl.h"

float complex clog10f(float complex z)
{
	float complex w = clogf(z);

	return CMPLXF(crealf(w) * (float)M_LOG10E, cimagf(w) * (float)M_LOG10E);
}
strong_alias(clog10f, __clog10f)
