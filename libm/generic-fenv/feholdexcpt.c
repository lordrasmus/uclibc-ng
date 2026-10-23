/* Store current floating-point environment and clear exceptions.
   Generic version for targets without exception flags.
   Copyright (C) 1997-2025 Free Software Foundation, Inc.
   Taken from the GNU C Library, math/feholdexcpt.c.

   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include <fenv.h>

int
feholdexcept (fenv_t *envp)
{
  /* No exception traps to disable and no state to save.  */
  (void) envp;
  return 0;
}
