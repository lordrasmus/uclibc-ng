/* Test exception in current environment.
   Generic version for targets without exception flags.
   Copyright (C) 1997-2025 Free Software Foundation, Inc.
   Taken from the GNU C Library, math/ftestexcept.c.

   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include <fenv.h>

int
fetestexcept (int excepts)
{
  (void) excepts;
  return 0;
}
