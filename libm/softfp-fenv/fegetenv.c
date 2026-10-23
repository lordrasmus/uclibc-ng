/* Store current floating-point environment.
   Soft-float version: the environment is kept by sfp-fenv.c and fed by
   libgcc's soft-fp.
   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include "sfp-fenv.h"

int
fegetenv (fenv_t *envp)
{
  __sfp_store_env (envp);
  return 0;
}
