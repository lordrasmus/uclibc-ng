/* Store current floating-point environment and clear exceptions.
   Soft-float version: the environment is kept by sfp-fenv.c and fed by
   libgcc's soft-fp.
   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include "sfp-fenv.h"

int
feholdexcept (fenv_t *envp)
{
  __sfp_store_env (envp);
  __SFP_EXCEPTS = 0;
  return 0;
}
