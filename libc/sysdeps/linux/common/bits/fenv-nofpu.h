/* The floating-point environment without an FPU to keep one, as in glibc's
   math/: rounding is always to nearest, and there are no exception flags.
   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB in this tarball.  */

#ifndef _FENV_H
# error "Never use <bits/fenv-nofpu.h> directly; include <fenv.h> instead."
#endif

/* No FE_INVALID and the like: a macro that is not defined tells programs
   that the exception is not supported.  */
#define FE_ALL_EXCEPT	0

/* The other modes are not defined, which tells programs they cannot be
   selected.  */
#define FE_TONEAREST	0

/* Type representing exception flags.  */
typedef unsigned int fexcept_t;

/* Type representing floating-point environment.  */
typedef struct
  {
    fexcept_t __excepts;
  }
fenv_t;

/* If the default argument is used we use this value.  */
#define FE_DFL_ENV	((const fenv_t *) -1l)

/* Type representing floating-point control modes.  */
typedef unsigned int femode_t;

/* Default floating-point control modes.  */
#define FE_DFL_MODE	((const femode_t *) -1L)
