/* Return current rounding direction.
   Generic version for targets that only round to nearest.
   Copyright (C) 1997-2025 Free Software Foundation, Inc.
   Taken from the GNU C Library, math/fegetround.c.

   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include <fenv.h>

int
fegetround (void)
{
  return FE_TONEAREST;
}
