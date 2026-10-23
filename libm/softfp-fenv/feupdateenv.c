/* Install given floating-point environment and raise exceptions.
   Soft-float version: the environment is kept by sfp-fenv.c and fed by
   libgcc's soft-fp.
   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include "sfp-fenv.h"

int
feupdateenv (const fenv_t *envp)
{
  unsigned int saved = __SFP_EXCEPTS;

#ifdef FE_NOMASK_ENV
  if (envp == FE_NOMASK_ENV)
    return 1;
#endif
  __sfp_load_env (envp);
  __SFP_EXCEPTS |= saved;
  return 0;
}
