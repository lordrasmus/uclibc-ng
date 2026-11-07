/*
 * vfork without a stack frame: the child runs on the parent's stack, so a
 * return address saved there by a C wrapper is gone when the parent resumes.
 *
 * Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
 * in this tarball.
 */

#include <sysdep.h>
#include <sys/syscall.h>

#ifdef __NR_vfork

PSEUDO(__vfork, vfork, 0)
	ret
PSEUDO_END(__vfork)

#else

/* asm-generic has no vfork syscall. */
ENTRY(__vfork)
	movi	$r0, 0x4111	/* CLONE_VM | CLONE_VFORK | SIGCHLD */
	movi	$r1, 0		/* stay on the parent's stack */
	b	__vfork_clone
END(__vfork)

	.hidden	__vfork_clone
PSEUDO(__vfork_clone, clone, 2)
	ret
PSEUDO_END(__vfork_clone)

#endif

weak_alias(__vfork, vfork)
libc_hidden_weak(vfork)
