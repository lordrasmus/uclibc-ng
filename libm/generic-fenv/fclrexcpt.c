/* Clear given exceptions in current floating-point environment.
   Generic version for targets without exception flags.
   Copyright (C) 1997-2025 Free Software Foundation, Inc.
   Taken from the GNU C Library, math/fclrexcpt.c.

   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include <fenv.h>

int
feclearexcept (int excepts)
{
  /* This always fails unless nothing needs to be done.  */
  return (excepts != 0);
}
