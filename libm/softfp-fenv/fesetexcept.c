/* Set given exception flags.
   Soft-float version: the environment is kept by sfp-fenv.c and fed by
   libgcc's soft-fp.
   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include "sfp-fenv.h"

int
fesetexcept (int excepts)
{
  __SFP_EXCEPTS |= excepts & FE_ALL_EXCEPT;
  return 0;
}
