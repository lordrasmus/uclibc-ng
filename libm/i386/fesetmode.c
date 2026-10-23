/* Install given floating-point control modes.  x87 only, as the rest of
   this port.
   Copyright (C) 2016-2025 Free Software Foundation, Inc.
   Taken from the GNU C Library, sysdeps/i386/fpu/fesetmode.c.

   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include <fenv.h>
#include <fpu_control.h>

int
fesetmode (const femode_t *modep)
{
  fpu_control_t cw;

  if (modep == FE_DFL_MODE)
    cw = _FPU_DEFAULT;
  else
    cw = modep->__control_word;
  _FPU_SETCW (cw);
  return 0;
}
