/* Private macros for accessing __jmp_buf contents.  BFIN version.  */
/*
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#ifdef __arch_v32
/* setjmp.S stores r0..r9 first and the stack pointer at word 14.  */
# define JB_SP		14
#else
# define JB_SP		1
#endif
