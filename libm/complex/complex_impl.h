/* The complex functions are musl's (src/complex, MIT; most of them come
   from FreeBSD's msun).  This maps what they take from musl's
   src/internal/complex_impl.h and libm.h onto uClibc.  */
#ifndef _COMPLEX_IMPL_H
#define _COMPLEX_IMPL_H

#include <complex.h>
#include <math.h>
#include <float.h>
#include <stdint.h>
#include "math_private.h"

#define hidden attribute_hidden

/* catrig.c: a NaN result built from both NaN inputs.  */
#define nan_mix(x, y) ((x) + (y))

/* clog.c: the 2sum and 2sumF steps from FreeBSD's math_private.h.  */
#define _2sum(a, b) do {	\
	__typeof(a) __s, __w;	\
				\
	__w = (a) + (b);	\
	__s = __w - (a);	\
	(b) = ((a) - (__w - __s)) + ((b) - __s); \
	(a) = __w;		\
} while (0)
#define _2sumF(a, b) do {	\
	__typeof(a) __w;	\
				\
	__w = (a) + (b);	\
	(b) = ((a) - __w) + (b); \
	(a) = __w;		\
} while (0)

#undef creal
#undef crealf
#undef creall
#undef cimag
#undef cimagf
#undef cimagl
#define creal(x) ((double)(x))
#define crealf(x) ((float)(x))
#define creall(x) ((long double)(x))
#define cimag(x) ((double)__imag__(x))
#define cimagf(x) ((float)__imag__(x))
#define cimagl(x) ((long double)__imag__(x))

#undef __CMPLX
#undef CMPLX
#undef CMPLXF
#undef CMPLXL
#define __CMPLX(x, y, t) \
	((union { _Complex t __z; t __xy[2]; }){.__xy = {(x),(y)}}.__z)
#define CMPLX(x, y) __CMPLX(x, y, double)
#define CMPLXF(x, y) __CMPLX(x, y, float)
#define CMPLXL(x, y) __CMPLX(x, y, long double)

hidden double complex __ldexp_cexp(double complex,int);
hidden float complex __ldexp_cexpf(float complex,int);

#endif
