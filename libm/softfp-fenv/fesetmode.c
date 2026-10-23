/* Install given floating-point control modes.
   Soft-float version: the environment is kept by sfp-fenv.c and fed by
   libgcc's soft-fp.
   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include "sfp-fenv.h"

int
fesetmode (const femode_t *modep)
{
  if (modep == FE_DFL_MODE)
    __SFP_ROUND = FE_TONEAREST;
  else if (__sfp_valid_round (*modep))
    __SFP_ROUND = *modep;
  else
    return -1;
  return 0;
}
