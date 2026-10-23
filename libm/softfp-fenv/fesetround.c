/* Set current rounding direction.
   Soft-float version: the environment is kept by sfp-fenv.c and fed by
   libgcc's soft-fp.
   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include "sfp-fenv.h"

int
fesetround (int round)
{
  if (!__sfp_valid_round (round))
    return 1;
  __SFP_ROUND = round;
  return 0;
}
