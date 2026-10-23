/* Store current floating-point control modes.  x87 only, as the rest of
   this port.
   Copyright (C) 2016-2025 Free Software Foundation, Inc.
   Taken from the GNU C Library, sysdeps/i386/fpu/fegetmode.c.

   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include <fenv.h>
#include <fpu_control.h>

int
fegetmode (femode_t *modep)
{
  _FPU_GETCW (modep->__control_word);
  return 0;
}
