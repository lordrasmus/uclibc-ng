/* Store current floating-point control modes, for a port whose only mode
   is the rounding direction and whose femode_t holds it.  Included from
   the port's directory.
   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include <fenv.h>

int
fegetmode (femode_t *modep)
{
  *modep = fegetround ();
  return 0;
}
