/* The state of the soft-float environment and the two functions libgcc's
   soft-fp calls, see sfp-fenv.h.
   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include "sfp-fenv.h"

/* m68k's libgcc keeps the environment itself (sfp-fenv.h).  */
#ifndef __m68k__

__thread struct __sfp_fenv __sfp_fenv
  attribute_hidden attribute_tls_model_ie = { 0, FE_TONEAREST };

int
__sfp_rounding_mode (void)
{
  return __sfp_fenv.round;
}

void
__sfp_raise_exceptions (int excepts)
{
  __sfp_fenv.excepts |= excepts & FE_ALL_EXCEPT;
}

#endif /* !__m68k__ */
