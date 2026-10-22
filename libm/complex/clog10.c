#include "complex_impl.h"

/* GNU extension.  Scaling clog keeps its relative accuracy, also in the
   real part where |z| is close to 1.  */
double complex clog10(double complex z)
{
	double complex w = clog(z);

	return CMPLX(creal(w) * M_LOG10E, cimag(w) * M_LOG10E);
}
libm_hidden_def(clog10)
/* <tgmath.h> calls it by this name.  */
strong_alias(clog10, __clog10)
