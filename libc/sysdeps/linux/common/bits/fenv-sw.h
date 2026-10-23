/* No hardware floating-point environment: keep one in software when
   libgcc's soft-fp reports to the C library, else offer the minimal one.
   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB in this tarball.  */

#ifndef _FENV_H
# error "Never use <bits/fenv-sw.h> directly; include <fenv.h> instead."
#endif

#ifdef __UCLIBC_LIBGCC_SFP_FENV__
# include <bits/fenv-softfp.h>
#else
# include <bits/fenv-nofpu.h>
#endif
