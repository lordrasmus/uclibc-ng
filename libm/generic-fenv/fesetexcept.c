/* Set given exception flags.
   Generic version for targets without exception flags.
   Copyright (C) 2016-2025 Free Software Foundation, Inc.
   Taken from the GNU C Library, math/fesetexcept.c.

   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include <fenv.h>

int
fesetexcept (int excepts)
{
  /* This always fails unless nothing needs to be done.  */
  return (excepts != 0);
}
