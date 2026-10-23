/* Install given floating-point environment.
   Soft-float version: the environment is kept by sfp-fenv.c and fed by
   libgcc's soft-fp.
   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include "sfp-fenv.h"

int
fesetenv (const fenv_t *envp)
{
#ifdef FE_NOMASK_ENV
  /* There are no traps to enable.  */
  if (envp == FE_NOMASK_ENV)
    return 1;
#endif
  __sfp_load_env (envp);
  return 0;
}
