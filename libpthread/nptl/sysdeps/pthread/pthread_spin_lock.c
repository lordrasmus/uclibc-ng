/* pthread_spin_lock -- lock a spin lock.  Generic version.
   Copyright (C) 2012-2026 Free Software Foundation, Inc.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <atomic.h>
#include "pthreadP.h"

/* Where the exchange is itself a CAS loop, one CAS does the job.  */
#ifdef __ATOMIC_EXCHANGE_IS_CAS
# define TRY_LOCK(l) (!atomic_compare_and_exchange_bool_acq ((l), 1, 0))
#else
# define TRY_LOCK(l) (atomic_exchange_acquire ((l), 1) == 0)
#endif

int
pthread_spin_lock (pthread_spinlock_t *lock)
{
  /* pthread_spinlock_t is volatile, and the atomic macros would carry that
     into their temporaries and spill them.  They need no volatile.  */
  int *l = (int *) lock;

  /* The first try mostly succeeds.  */
  if (__builtin_expect (TRY_LOCK (l), 1))
    return 0;

  do
    {
      /* Wait with plain reads until the lock looks free, rather than
	 hammering it with read-modify-writes.  */
      do
	atomic_delay ();
      while (*lock != 0);
    }
  while (!TRY_LOCK (l));

  return 0;
}
