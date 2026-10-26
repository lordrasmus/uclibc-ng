/*
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <errno.h>

/* This routine is jumped to by all the syscall handlers, to stash
   an error number into errno.  */
long attribute_hidden __syscall_error (void)
{
	register long v0 __asm__("$0");
	int err_no;

	/* Copy $0 first: with TLS, reaching errno reads the thread pointer
	   through rduniq, which returns it in $0 too.  Taking v0 as an input
	   orders the copy before that.  */
	__asm__ ("mov %1,%0" : "=r" (err_no) : "r" (v0));
	__set_errno (err_no);
	return -1;
}
