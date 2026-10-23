/* Enable floating-point exceptions.
   Soft-float version: the environment is kept by sfp-fenv.c and fed by
   libgcc's soft-fp.
   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include "sfp-fenv.h"

int
feenableexcept (int excepts)
{
  /* Nothing traps; enabling anything fails.  */
  return (excepts & FE_ALL_EXCEPT) != 0 ? -1 : 0;
}
