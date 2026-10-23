/* Install given floating-point control modes, for a port whose only mode
   is the rounding direction and whose femode_t holds it.  Included from
   the port's directory.
   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include <fenv.h>

int
fesetmode (const femode_t *modep)
{
  return fesetround (modep == FE_DFL_MODE ? FE_TONEAREST : (int) *modep)
	 ? -1 : 0;
}
