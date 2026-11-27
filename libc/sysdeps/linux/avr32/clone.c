/*
 * Copyright (C) 2004 Atmel Corporation
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License.  See the file "COPYING.LIB" in the main directory of this
 * archive for more details.
 */
#include <sched.h>
#include <errno.h>
#include <stdarg.h>
#include <sys/syscall.h>
#include <unistd.h>

/*
 * I don't know if we can be absolutely certain that the fn and arg
 * parameters are preserved when returning as the child. If the
 * compiler stores them in registers (r0-r7), they should be.
 */
int clone(int (*fn)(void *arg), void *child_stack, int flags, void *arg, ...)
{
	register int (*_fn)(void *arg) = fn;
	register void *_arg = arg;
	int *ptid, *ctid;
	void *tls;
	va_list ap;
	int ret;

	if (!fn || !child_stack) {
		__set_errno(EINVAL);
		return -1;
	}

	va_start(ap, arg);
	ptid = va_arg(ap, int *);
	tls = va_arg(ap, void *);
	ctid = va_arg(ap, int *);
	va_end(ap);

	ret = INLINE_SYSCALL(clone, 5, flags, child_stack, ptid, ctid, tls);
	if (ret != 0)
		return ret;	/* parent: the child's id, or -1 with errno set */

	/* The exit system call, not _exit: that is exit_group with threads
	   and would end every thread of a CLONE_THREAD child.  Call fn
	   first: as an argument of INLINE_SYSCALL the call would clobber
	   the syscall number already loaded into r8.  */
	ret = _fn(_arg);
	INLINE_SYSCALL(exit, 1, ret);
	for (;;)
		;
}
