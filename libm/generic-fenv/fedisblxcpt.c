/* Disable floating-point exceptions.
   Generic version for targets without exception traps.
   Copyright (C) 2000-2025 Free Software Foundation, Inc.
   Taken from the GNU C Library, math/fedisblxcpt.c.

   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include <fenv.h>

int
fedisableexcept (int excepts)
{
  /* All exception traps are disabled.  */
  (void) excepts;
  return 0;
}
