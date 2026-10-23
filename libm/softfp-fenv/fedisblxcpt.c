/* Disable floating-point exceptions.
   Soft-float version: the environment is kept by sfp-fenv.c and fed by
   libgcc's soft-fp.
   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include "sfp-fenv.h"

int
fedisableexcept (int excepts)
{
  (void) excepts;
  return 0;
}
