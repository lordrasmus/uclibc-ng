/* Set floating-point environment exception handling.
   Soft-float version: the environment is kept by sfp-fenv.c and fed by
   libgcc's soft-fp.
   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include "sfp-fenv.h"

int
fesetexceptflag (const fexcept_t *flagp, int excepts)
{
  excepts &= FE_ALL_EXCEPT;
  __SFP_EXCEPTS = (__SFP_EXCEPTS & ~excepts) | (*flagp & excepts);
  return 0;
}
