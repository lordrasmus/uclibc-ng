/*
 * The value behind math_errhandling for uClibc
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <math.h>
#include <fenv.h>

/* MATH_ERREXCEPT needs the flags for domain errors, poles and overflow;
   the generic fenv of soft-float targets has none, m68k without FPU no
   invalid.  */
#if defined FE_INVALID && defined FE_DIVBYZERO && defined FE_OVERFLOW
const int __math_errhandling = MATH_ERREXCEPT;
#else
const int __math_errhandling = 0;
#endif
