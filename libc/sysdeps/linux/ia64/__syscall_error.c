/* Wrapper for setting errno.
 *
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <errno.h>
#include <features.h>

/* This routine is jumped to by all the syscall handlers, to stash
 * an error number into errno.  */
long __syscall_error(void) attribute_hidden;
long __syscall_error(void)
{
	int err_no;

	/* Read r8 before anything else: __set_errno may be a call that
	   returns in r8 itself.  */
	__asm__ __volatile__ ("mov %0 = r8" : "=r" (err_no));
	__set_errno(err_no);
	return -1;
}
