/* Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.  */

#include <sched.h>
#include <signal.h>
#include <sysdep.h>
#include <tls.h>

/* hppa takes clone (flags, child stack, parent tid, tls, child tid)
   (CLONE_BACKWARDS).  */
#define ARCH_FORK() \
  INLINE_SYSCALL (clone, 5,						\
		  CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID | SIGCHLD,	\
		  NULL, NULL, NULL, &THREAD_SELF->tid)

#include "../fork.c"
