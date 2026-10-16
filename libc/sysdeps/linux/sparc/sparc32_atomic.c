/*
 * Copyright (C) 2026 Ramin Moussavi <lordrasmus@gmail.com>
 *
 * Licensed under the LGPL v2.1 or later, see the file COPYING.LIB in this tarball.
 */

#include <atomic.h>

#ifndef __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4

#include <setjmp.h>
#include <signal.h>
#include <string.h>

int __sparc32_atomic_casa;

static sigjmp_buf probe_env;

static void probe_sigill (int sig)
{
	(void) sig;
	siglongjmp (probe_env, 1);
}

/* Try one casa on a word of our own; the SIGILL handler catches a CPU
   that has none and a kernel that does not emulate it.  */
static int casa_works (int kind)
{
	unsigned int word = 0, prev;

	if (sigsetjmp (probe_env, 1))
		return 0;
	if (kind == 1)
		prev = __sparc32_casa_word ("0xcde04145", &word, 1, 0);
	else
		prev = __sparc32_casa_word ("0xcde05005", &word, 1, 0);
	return prev == 0 && word == 1;
}

/* Called by __uClibc_init once, while the process has one thread.  */
void __sparc32_atomic_init (void)
{
	struct sigaction sa, old;
	int kind = 0;

	memset (&sa, 0, sizeof (sa));
	sa.sa_handler = probe_sigill;
	if (sigaction (SIGILL, &sa, &old) != 0)
		return;

	/* 0x0a first: it runs natively on LEON parts that have casa.  A
	   64-bit kernel refuses it from user mode, but takes 0x80.  */
	if (casa_works (1))
		kind = 1;
	else if (casa_works (2))
		kind = 2;

	sigaction (SIGILL, &old, NULL);
	__sparc32_atomic_casa = kind;
}

#endif
