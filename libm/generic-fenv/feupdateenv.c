/* Install given floating-point environment and raise exceptions.
   Generic version for targets without exception flags.
   Copyright (C) 1997-2025 Free Software Foundation, Inc.
   Taken from the GNU C Library, math/feupdateenv.c.

   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include <fenv.h>

int
feupdateenv (const fenv_t *envp)
{
#ifdef FE_NOMASK_ENV
  if (envp == FE_NOMASK_ENV)
    return 1;
#endif
  /* Nothing to do.  */
  (void) envp;
  return 0;
}
