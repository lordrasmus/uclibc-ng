/* Set floating-point environment exception handling.
   Generic version for targets without exception flags.
   Copyright (C) 1997-2025 Free Software Foundation, Inc.
   Taken from the GNU C Library, math/fsetexcptflg.c.

   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include <fenv.h>

int
fesetexceptflag (const fexcept_t *flagp, int excepts)
{
  /* This always succeeds, as all exceptions are always clear
     (including in the saved state) so nothing needs to be done.  */
  (void) flagp;
  (void) excepts;
  return 0;
}
