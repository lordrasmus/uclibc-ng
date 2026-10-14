/* Low level locking, sparc version.
   Copyright (C) 2003, 2004, 2006, 2007, 2008, 2009
   Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Jakub Jelinek <jakub@redhat.com>, 2003.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If
   not, see <http://www.gnu.org/licenses/>.  */

#ifndef _SPARC_LOWLEVELLOCK_H
#define _SPARC_LOWLEVELLOCK_H	1

/* Take the futex plumbing, the constants and the tid helpers from the
   generic header; none of that is architecture specific.  Only the few
   operations that touch an lll word itself are overridden below.  */
#include_next <lowlevellock.h>

#ifndef __ASSEMBLER__

/* DO NOT DELETE THIS FILE.  It is not a copy of the generic header, and
   lowlevellock.c in this directory does not work without it.

   A sparc32 without a compare-and-swap instruction emulates the atomics
   with locks, and bits/atomic.h has two kinds that know nothing of each
   other:

     - the plain forms take one of the 64 address hashed locks in
       __sparc32_atomic_locks, then load, compare and store the word;
     - the _24 forms keep their lock in the top byte of the very word
       they protect, which is why they only carry 24 bits of value.

   An lll word must be touched by one of the two on every path that
   touches it.  lowlevellock.c reaches it with the _24 forms, so lock,
   unlock and trylock have to use them as well.

   Mixing them leaves the mutex locked forever with nobody holding it:
   the unlocker reads the word and writes 0 while holding the table
   lock, a waiter is at that moment inside its own section holding the
   top byte, and the waiter's store of 2 lands after the unlocker's
   store of 0.  The wake has already gone out and found nobody asleep,
   so the word stays at 2 and every later arrival sleeps on it.
   Observed as __lock == 2 with __owner == 0 and every thread in
   FUTEX_WAIT.

   The robust forms below stay on the plain atomics on purpose: their
   word carries the owner tid together with FUTEX_WAITERS in bit 31 and
   does not fit in 24 bits.  */

/* FUTEX_WAKE_OP has the kernel clear the lock word, and the kernel
   knows about neither of the two schemes above.  Report failure so
   pthread_cond_signal wakes and unlocks in two separate steps.  */
#ifdef __sparc32_atomic_do_lock
# undef lll_futex_wake_unlock
# define lll_futex_wake_unlock(futexp, nr_wake, nr_wake2, futexp2, private) 1
#endif

#undef lll_trylock
#define lll_trylock(lock)	\
  atomic_compare_and_exchange_val_24_acq (&(lock), 1, 0)

#undef lll_cond_trylock
#define lll_cond_trylock(lock)	\
  atomic_compare_and_exchange_val_24_acq (&(lock), 2, 0)

#undef __lll_lock
#define __lll_lock(futex, private)					      \
  ((void) ({								      \
    int *__futex = (futex);						      \
    if (__builtin_expect (atomic_compare_and_exchange_val_24_acq (__futex,    \
								  1, 0), 0))  \
      {									      \
	if (__builtin_constant_p (private) && (private) == LLL_PRIVATE)	      \
	  __lll_lock_wait_private (__futex);				      \
	else								      \
	  __lll_lock_wait (__futex, private);				      \
      }									      \
  }))
#undef lll_lock
#define lll_lock(futex, private) __lll_lock (&(futex), private)

/* There is no 24 bit exchange, so acquire with a compare-and-exchange
   against 0.  A word that was already taken is left as it is and
   __lll_lock_wait marks it contended.  */
#undef __lll_cond_lock
#define __lll_cond_lock(futex, private)					      \
  ((void) ({								      \
    int *__futex = (futex);						      \
    if (__builtin_expect (atomic_compare_and_exchange_val_24_acq (__futex,    \
								  2, 0), 0))  \
      __lll_lock_wait (__futex, private);				      \
  }))
#undef lll_cond_lock
#define lll_cond_lock(futex, private) __lll_cond_lock (&(futex), private)

#undef __lll_timedlock
#define __lll_timedlock(futex, abstime, private)			      \
  ({									      \
     int *__futex = (futex);						      \
     int __val = 0;							      \
									      \
     if (__builtin_expect (atomic_compare_and_exchange_val_24_acq (__futex,   \
								   1, 0), 0)) \
       __val = __lll_timedlock_wait (__futex, abstime, private);	      \
     __val;								      \
  })
#undef lll_timedlock
#define lll_timedlock(futex, abstime, private) \
  __lll_timedlock (&(futex), abstime, private)

#undef __lll_unlock
#define __lll_unlock(futex, private)					      \
  (void)								      \
    ({ int *__futex = (futex);						      \
       int __oldval = atomic_exchange_24_rel (__futex, 0);		      \
       if (__builtin_expect (__oldval > 1, 0))				      \
	 lll_futex_wake (__futex, 1, private);				      \
    })
#undef lll_unlock
#define lll_unlock(futex, private) __lll_unlock (&(futex), private)

#endif /* __ASSEMBLER__ */

#endif	/* lowlevellock.h */
