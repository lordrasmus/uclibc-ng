/* Store current representation for exceptions.
   Copyright (C) 1997-2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library.  If not, see
   <https://www.gnu.org/licenses/>.  */

#include "fenv_libc.h"

int
fegetexceptflag (fexcept_t *flagp, int excepts)
{
  unsigned long int tmp;

  /* Get the current state.  */
  tmp = __ieee_get_fp_control();

  /* Return that portion that corresponds to the requested exceptions. */
  *flagp = tmp & excepts & SWCR_STATUS_MASK;

  /* Success.  */
  return 0;
}
