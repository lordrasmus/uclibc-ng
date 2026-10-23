/* Test exception in current environment.
   Soft-float version: the environment is kept by sfp-fenv.c and fed by
   libgcc's soft-fp.
   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include "sfp-fenv.h"

int
fetestexcept (int excepts)
{
  return __SFP_EXCEPTS & excepts & FE_ALL_EXCEPT;
}
