/*
 * Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
 * in this tarball.
 */

#include <sys/syscall.h>

#ifndef __NR_SYSCALL_BASE

# include "../../../signal/sigaction.c"

#else

/* Andes BSP kernel (Linux 3.4): its struct sigaction has sa_restorer in
   front of sa_mask, ours keeps it behind. */

# include <signal.h>
# include <string.h>
# include <bits/kernel_sigaction.h>

# define SA_RESTORER	0x04000000

extern void __default_sa_restorer(void) attribute_hidden;
extern void __default_rt_sa_restorer(void) attribute_hidden;

int
__libc_sigaction(int sig, const struct sigaction *act, struct sigaction *oact)
{
	struct kernel_sigaction kact, koact;
	int result;

	if (act) {
		kact.k_sa_handler = act->sa_handler;
		kact.sa_flags = act->sa_flags;
		if (kact.sa_flags & SA_RESTORER) {
			kact.sa_restorer = act->sa_restorer;
		} else {
			kact.sa_restorer = (kact.sa_flags & SA_SIGINFO)
				? __default_rt_sa_restorer
				: __default_sa_restorer;
			kact.sa_flags |= SA_RESTORER;
		}
		memcpy(&kact.sa_mask, &act->sa_mask, sizeof(kact.sa_mask));
	}
	result = __syscall_rt_sigaction(sig,
			act ? (const struct sigaction *) &kact : NULL,
			oact ? (struct sigaction *) &koact : NULL,
			sizeof(kact.sa_mask));
	if (oact && result >= 0) {
		oact->sa_handler = koact.k_sa_handler;
		oact->sa_flags = koact.sa_flags;
		oact->sa_restorer = koact.sa_restorer;
		memcpy(&oact->sa_mask, &koact.sa_mask, sizeof(oact->sa_mask));
	}
	return result;
}

# ifndef LIBC_SIGACTION
#  ifndef __UCLIBC_HAS_THREADS__
strong_alias(__libc_sigaction,sigaction)
libc_hidden_def(sigaction)
#  elif !defined __UCLIBC_HAS_THREADS_NATIVE__
weak_alias(__libc_sigaction,sigaction)
libc_hidden_weak(sigaction)
#  endif
# endif

#endif
