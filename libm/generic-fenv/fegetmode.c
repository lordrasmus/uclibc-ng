/* Store current floating-point control modes.
   Generic version for targets without control modes.
   Copyright (C) 2016-2025 Free Software Foundation, Inc.
   Taken from the GNU C Library, math/fegetmode.c.

   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include <fenv.h>

int
fegetmode (femode_t *modep)
{
  /* Nothing to do.  */
  (void) modep;
  return 0;
}
