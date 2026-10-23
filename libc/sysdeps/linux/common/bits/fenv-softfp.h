/* The floating-point environment of soft-float code, kept in software by
   libm/softfp-fenv and fed by libgcc's soft-fp.  The values are the ones
   libgcc reports in (libgcc/soft-fp-libc-fenv.h), the same on every
   target: those of the RISC-V fcsr.
   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB in this tarball.  */

#ifndef _FENV_H
# error "Never use <bits/fenv-softfp.h> directly; include <fenv.h> instead."
#endif

enum
  {
    FE_INEXACT =
#define FE_INEXACT	0x01
      FE_INEXACT,
    FE_UNDERFLOW =
#define FE_UNDERFLOW	0x02
      FE_UNDERFLOW,
    FE_OVERFLOW =
#define FE_OVERFLOW	0x04
      FE_OVERFLOW,
    FE_DIVBYZERO =
#define FE_DIVBYZERO	0x08
      FE_DIVBYZERO,
    FE_INVALID =
#define FE_INVALID	0x10
      FE_INVALID
  };

#define FE_ALL_EXCEPT \
	(FE_INEXACT | FE_DIVBYZERO | FE_UNDERFLOW | FE_OVERFLOW | FE_INVALID)

enum
  {
    FE_TONEAREST =
#define FE_TONEAREST	0x0
      FE_TONEAREST,
    FE_TOWARDZERO =
#define FE_TOWARDZERO	0x1
      FE_TOWARDZERO,
    FE_DOWNWARD =
#define FE_DOWNWARD	0x2
      FE_DOWNWARD,
    FE_UPWARD =
#define FE_UPWARD	0x3
      FE_UPWARD
  };

/* Type representing exception flags.  */
typedef unsigned int fexcept_t;

/* Type representing floating-point environment: the flags, and the
   rounding mode above them.  */
typedef unsigned int fenv_t;

/* If the default argument is used we use this value.  */
#define FE_DFL_ENV	((const fenv_t *) -1)

/* Type representing floating-point control modes.  */
typedef unsigned int femode_t;

/* Default floating-point control modes.  */
#define FE_DFL_MODE	((const femode_t *) -1L)
