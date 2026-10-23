/* Enable floating-point exceptions.
   Generic version for targets without exception traps.
   Copyright (C) 2000-2025 Free Software Foundation, Inc.
   Taken from the GNU C Library, math/feenablxcpt.c.

   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include <fenv.h>

int
feenableexcept (int excepts)
{
  /* Signal failure if any exception traps are to be enabled.  */
  return (excepts != 0) ? -1 : 0;
}
