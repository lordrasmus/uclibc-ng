/* Set given exception flags.  x87 only, as the rest of this port.
   Copyright (C) 2016-2025 Free Software Foundation, Inc.
   Taken from the GNU C Library, sysdeps/i386/fpu/fesetexcept.c.

   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#include <fenv.h>

int
fesetexcept (int excepts)
{
  fenv_t temp;

  excepts &= FE_ALL_EXCEPT;

  /* Note: fnstenv masks all floating-point exceptions until the fldenv
     or fldcw below.  */
  __asm__ ("fnstenv %0" : "=m" (temp));

  /* Set relevant flags.  */
  temp.__status_word |= excepts;

  if ((~temp.__control_word) & excepts)
    {
      /* Setting the exception flags may trigger a trap (at the next
	 floating-point instruction, but that does not matter).
	 ISO C23 (7.6.4.4) does not allow it.  */
      __asm__ volatile ("fldcw %0" : : "m" (temp.__control_word));
      return -1;
    }

  /* Store the new status word (along with the rest of the environment).  */
  __asm__ ("fldenv %0" : : "m" (temp));

  return 0;
}
