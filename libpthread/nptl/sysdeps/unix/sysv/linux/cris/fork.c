/* Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.  */

#include <sched.h>
#include <signal.h>
#include <sysdep.h>
#include <tls.h>

/* cris takes clone (child stack, flags, parent tid, child tid, tls)
   (CLONE_BACKWARDS2).  */
#define ARCH_FORK() \
  INLINE_SYSCALL (clone, 5,						\
		  NULL, CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID | SIGCHLD, \
		  NULL, &THREAD_SELF->tid, NULL)

#include "../fork.c"
