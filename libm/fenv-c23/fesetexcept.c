/* Set given exception flags without raising them, built from the port's
   own functions so that fexcept_t keeps its representation: feraiseexcept
   produces the flags while feholdexcept has the traps off, and
   fesetexceptflag installs them into the restored environment, which
   does not trap.  Included from the port's directory.
   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include <fenv.h>

int
fesetexcept (int excepts)
{
  fenv_t env;
  fexcept_t flags;

  excepts &= FE_ALL_EXCEPT;
  if (excepts == 0)
    return 0;
  if (feholdexcept (&env) != 0)
    return -1;
  feraiseexcept (excepts);
  fegetexceptflag (&flags, excepts);
  if (fesetenv (&env) != 0)
    return -1;
  return fesetexceptflag (&flags, excepts) != 0 ? -1 : 0;
}
