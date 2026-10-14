/* Atomic operations.  sparc32 version.
   Copyright (C) 2003, 2004, 2006 Free Software Foundation, Inc.
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
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _BITS_ATOMIC_H
#define _BITS_ATOMIC_H	1

#include <stdint.h>

typedef int8_t atomic8_t;
typedef uint8_t uatomic8_t;
typedef int_fast8_t atomic_fast8_t;
typedef uint_fast8_t uatomic_fast8_t;

typedef int16_t atomic16_t;
typedef uint16_t uatomic16_t;
typedef int_fast16_t atomic_fast16_t;
typedef uint_fast16_t uatomic_fast16_t;

typedef int32_t atomic32_t;
typedef uint32_t uatomic32_t;
typedef int_fast32_t atomic_fast32_t;
typedef uint_fast32_t uatomic_fast32_t;

typedef int64_t atomic64_t;
typedef uint64_t uatomic64_t;
typedef int_fast64_t atomic_fast64_t;
typedef uint_fast64_t uatomic_fast64_t;

typedef intptr_t atomicptr_t;
typedef uintptr_t uatomicptr_t;
typedef intmax_t atomic_max_t;
typedef uintmax_t uatomic_max_t;


/* Without compare-and-swap the primitives below are built from a lock,
   and a signal handler that reaches an atomic while the code it
   interrupted holds that lock spins forever.  Keep signals blocked
   across the window; they stay pending and arrive once the mask is
   restored.  With the instruction the macros vanish.  */
#if defined __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4

typedef int __sparc32_atomic_sigmask_t;
/* The casts keep the save area from reading as an unused variable at the
   call sites, which stay the same for both branches.  */
# define __sparc32_atomic_block_signals(save)	((void) (save))
# define __sparc32_atomic_restore_signals(save)	((void) (save))

#else

# include <sys/syscall.h>

typedef unsigned long __sparc32_atomic_sigmask_t[2];	/* _NSIG is 64 */

/* sparc numbers these differently from every other port: SIG_BLOCK is 1
   and SIG_SETMASK is 4.  Spelled out because <signal.h> must not be
   pulled into a header this low.  */
# define __sparc32_atomic_sigprocmask(how, set, oldset) \
  do								      \
    {								      \
      register long __spg1 __asm__ ("g1") = __NR_rt_sigprocmask;      \
      register long __spo0 __asm__ ("o0") = (how);		      \
      register long __spo1 __asm__ ("o1") = (long) (set);	      \
      register long __spo2 __asm__ ("o2") = (long) (oldset);	      \
      register long __spo3 __asm__ ("o3")			      \
	= sizeof (__sparc32_atomic_sigmask_t);			      \
      __asm__ __volatile__ ("t 0x10"				      \
			  : "=r" (__spo0)			      \
			  : "r" (__spg1), "0" (__spo0), "r" (__spo1), \
			    "r" (__spo2), "r" (__spo3)		      \
			  : "cc", "memory");			      \
    }								      \
  while (0)

# define __sparc32_atomic_block_signals(save) \
  do								      \
    {								      \
      const unsigned long __spall[2] = { ~0UL, ~0UL };		      \
      __sparc32_atomic_sigprocmask (1 /* SIG_BLOCK */, __spall, (save)); \
    }								      \
  while (0)

# define __sparc32_atomic_restore_signals(save) \
  __sparc32_atomic_sigprocmask (4 /* SIG_SETMASK */, (save), 0)

#endif

/* A lock kept in the top byte of the word it protects.  The sparc specific
   sem_* and pthread_barrier_* still name these in the branch they take
   without a compare-and-swap instruction, so they stay out of the choice
   below; with one, that branch is dead and the compiler drops it.
   Callers wrap the window in __sparc32_atomic_block_signals, since the
   24-bit forms drop the lock by storing the new value.  */
#define __sparc32_atomic_do_lock24(addr) \
  do								      \
    {								      \
      unsigned int __old_lock;					      \
      do							      \
	__asm__ __volatile__ ("ldstub %1, %0"			      \
			  : "=r" (__old_lock), "=m" (*(addr))	      \
			  : "m" (*(addr))			      \
			  : "memory");				      \
      while (__old_lock);					      \
    }								      \
  while (0)

#define __sparc32_atomic_do_unlock24(addr) \
  do								      \
    {								      \
      *(char *) (addr) = 0;					      \
      __asm__ __volatile__ ("" ::: "memory");			      \
    }								      \
  while (0)

#if defined __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4

/* The sparc specific sem_* and pthread_barrier_* ask this before choosing
   between a plain atomic and the in-word lock.  The lock is there because
   the emulation below keeps its own locks per process, so it is not atomic
   between processes and cannot carry a pshared semaphore.  A real
   instruction can, so answer truthfully and those files take the path they
   have always had for v9.  */
# define __atomic_is_v9 1

/* leon3 and later, v8plus and v9 have a compare-and-swap instruction, and
   the compiler emits it inline.  Take it.  Besides being one instruction
   instead of a lock round trip, it is the only form that may be used from
   a signal handler: the emulation below takes a lock, and a handler that
   arrives while the interrupted code holds it spins for an owner that can
   never run again.  uClibc-ng walks into that itself -- sigcancel_handler
   and sighandler_setxid both compare-and-swap cancelhandling, the very
   word the code they interrupt is working on.

   Note that the architecture macro does not tell these apart: -mcpu=leon3
   defines __sparc_v8__ just as -mcpu=v8 does.  The question that matters
   is whether the compiler has a real CAS, so ask exactly that.  */

# define atomic_compare_and_exchange_val_acq(mem, newval, oldval) \
  __sync_val_compare_and_swap (mem, oldval, newval)

/* Mind the sense: this one reports failure, not success.  */
# define atomic_compare_and_exchange_bool_acq(mem, newval, oldval) \
  (! __sync_bool_compare_and_swap (mem, oldval, newval))

# define atomic_exchange_acq(mem, newval) \
  __atomic_exchange_n (mem, newval, __ATOMIC_ACQUIRE)

# define atomic_exchange_rel(mem, newval) \
  __atomic_exchange_n (mem, newval, __ATOMIC_RELEASE)

# define atomic_exchange_and_add(mem, value) \
  __sync_fetch_and_add (mem, value)

/* The _24 forms exist only because the emulation below needs a spare byte
   to keep its lock in, so it borrows the top one.  With a real instruction
   the whole word is available and these are ordinary 32-bit operations.  */
# define atomic_compare_and_exchange_val_24_acq(mem, newval, oldval) \
  __sync_val_compare_and_swap (mem, oldval, newval)

# define atomic_exchange_24_rel(mem, newval) \
  __atomic_exchange_n (mem, newval, __ATOMIC_RELEASE)

#else

/* We have no compare and swap, just test and set.
   The following implementation contends on 64 global locks
   per library and assumes no variable will be accessed using atomic.h
   macros from two different libraries.  */

__make_section_unallocated
  (".gnu.linkonce.b.__sparc32_atomic_locks, \"aw\", %nobits");

volatile unsigned char __sparc32_atomic_locks[64]
  __attribute__ ((nocommon, section (".gnu.linkonce.b.__sparc32_atomic_locks"
				     __sec_comment),
		  visibility ("hidden")));

#define __sparc32_atomic_do_lock(addr) \
  do								      \
    {								      \
      unsigned int __old_lock;					      \
      unsigned int __idx = (((long) addr >> 2) ^ ((long) addr >> 12)) \
			   & 63;				      \
      do							      \
	__asm__ __volatile__ ("ldstub %1, %0"			      \
			  : "=r" (__old_lock),			      \
			    "=m" (__sparc32_atomic_locks[__idx])      \
			  : "m" (__sparc32_atomic_locks[__idx])	      \
			  : "memory");				      \
      while (__old_lock);					      \
    }								      \
  while (0)

#define __sparc32_atomic_do_unlock(addr) \
  do								      \
    {								      \
      __sparc32_atomic_locks[(((long) addr >> 2)		      \
			      ^ ((long) addr >> 12)) & 63] = 0;	      \
      __asm__ __volatile__ ("" ::: "memory");			      \
    }								      \
  while (0)

#ifndef SHARED
# define __v9_compare_and_exchange_val_32_acq(mem, newval, oldval) \
({									      \
  register __typeof (*(mem)) __acev_tmp __asm__ ("%g6");			      \
  register __typeof (mem) __acev_mem __asm__ ("%g1") = (mem);		      \
  register __typeof (*(mem)) __acev_oldval __asm__ ("%g5");		      \
  __acev_tmp = (newval);						      \
  __acev_oldval = (oldval);						      \
  /* .word 0xcde05005 is cas [%g1], %g5, %g6.  Can't use cas here though,     \
     because as will then mark the object file as V8+ arch.  */		      \
  __asm__ __volatile__ (".word 0xcde05005"					      \
		    : "+r" (__acev_tmp), "=m" (*__acev_mem)		      \
		    : "r" (__acev_oldval), "m" (*__acev_mem),		      \
		      "r" (__acev_mem) : "memory");			      \
  __acev_tmp; })
#endif

/* The only basic operation needed is compare and exchange.  */
#define __v7_compare_and_exchange_val_acq(mem, newval, oldval) \
  ({ __typeof (mem) __acev_memp = (mem);			      \
     __typeof (*mem) __acev_ret;				      \
     __typeof (*mem) __acev_newval = (newval);			      \
     __sparc32_atomic_sigmask_t __acev_save;			      \
								      \
     __sparc32_atomic_block_signals (__acev_save);		      \
     __sparc32_atomic_do_lock (__acev_memp);			      \
     __acev_ret = *__acev_memp;					      \
     if (__acev_ret == (oldval))				      \
       *__acev_memp = __acev_newval;				      \
     __sparc32_atomic_do_unlock (__acev_memp);			      \
     __sparc32_atomic_restore_signals (__acev_save);		      \
     __acev_ret; })

#define __v7_compare_and_exchange_bool_acq(mem, newval, oldval) \
  ({ __typeof (mem) __aceb_memp = (mem);			      \
     int __aceb_ret;						      \
     __typeof (*mem) __aceb_newval = (newval);			      \
     __sparc32_atomic_sigmask_t __aceb_save;			      \
								      \
     __sparc32_atomic_block_signals (__aceb_save);		      \
     __sparc32_atomic_do_lock (__aceb_memp);			      \
     __aceb_ret = 0;						      \
     if (*__aceb_memp == (oldval))				      \
       *__aceb_memp = __aceb_newval;				      \
     else							      \
       __aceb_ret = 1;						      \
     __sparc32_atomic_do_unlock (__aceb_memp);			      \
     __sparc32_atomic_restore_signals (__aceb_save);		      \
     __aceb_ret; })

#define __v7_exchange_acq(mem, newval) \
  ({ __typeof (mem) __acev_memp = (mem);			      \
     __typeof (*mem) __acev_ret;				      \
     __typeof (*mem) __acev_newval = (newval);			      \
     __sparc32_atomic_sigmask_t __acev_save;			      \
								      \
     __sparc32_atomic_block_signals (__acev_save);		      \
     __sparc32_atomic_do_lock (__acev_memp);			      \
     __acev_ret = *__acev_memp;					      \
     *__acev_memp = __acev_newval;				      \
     __sparc32_atomic_do_unlock (__acev_memp);			      \
     __sparc32_atomic_restore_signals (__acev_save);		      \
     __acev_ret; })

#define __v7_exchange_and_add(mem, value) \
  ({ __typeof (mem) __acev_memp = (mem);			      \
     __typeof (*mem) __acev_ret;				      \
     __sparc32_atomic_sigmask_t __acev_save;			      \
								      \
     __sparc32_atomic_block_signals (__acev_save);		      \
     __sparc32_atomic_do_lock (__acev_memp);			      \
     __acev_ret = *__acev_memp;					      \
     *__acev_memp = __acev_ret + (value);			      \
     __sparc32_atomic_do_unlock (__acev_memp);			      \
     __sparc32_atomic_restore_signals (__acev_save);		      \
     __acev_ret; })

/* Special versions, which guarantee that top 8 bits of all values
   are cleared and use those bits as the ldstub lock.  This is a second,
   independent lock: a word reached with these is NOT protected against
   a plain form above, which locks the table instead and stores the full
   32 bits.  Whoever touches a word must pick one kind and keep to it on
   every path -- see libpthread/nptl/sysdeps/unix/sysv/linux/sparc/
   lowlevellock.h, which exists for exactly that reason.  */
#define __v7_compare_and_exchange_val_24_acq(mem, newval, oldval) \
  ({ __typeof (mem) __acev_memp = (mem);			      \
     __typeof (*mem) __acev_ret;				      \
     __typeof (*mem) __acev_newval = (newval);			      \
     __sparc32_atomic_sigmask_t __acev_save;			      \
								      \
     __sparc32_atomic_block_signals (__acev_save);		      \
     __sparc32_atomic_do_lock24 (__acev_memp);			      \
     __acev_ret = *__acev_memp & 0xffffff;			      \
     if (__acev_ret == (oldval))				      \
       *__acev_memp = __acev_newval;				      \
     else							      \
       __sparc32_atomic_do_unlock24 (__acev_memp);		      \
     __asm__ __volatile__ ("" ::: "memory");			      \
     __sparc32_atomic_restore_signals (__acev_save);		      \
     __acev_ret; })

#define __v7_exchange_24_rel(mem, newval) \
  ({ __typeof (mem) __acev_memp = (mem);			      \
     __typeof (*mem) __acev_ret;				      \
     __typeof (*mem) __acev_newval = (newval);			      \
     __sparc32_atomic_sigmask_t __acev_save;			      \
								      \
     __sparc32_atomic_block_signals (__acev_save);		      \
     __sparc32_atomic_do_lock24 (__acev_memp);			      \
     __acev_ret = *__acev_memp & 0xffffff;			      \
     *__acev_memp = __acev_newval;				      \
     __asm__ __volatile__ ("" ::: "memory");			      \
     __sparc32_atomic_restore_signals (__acev_save);		      \
     __acev_ret; })

#ifdef SHARED

/* When dynamically linked, we assume pre-v9 libraries are only ever
   used on pre-v9 CPU.  */
# define __atomic_is_v9 0

# define atomic_compare_and_exchange_val_acq(mem, newval, oldval) \
  __v7_compare_and_exchange_val_acq (mem, newval, oldval)

# define atomic_compare_and_exchange_bool_acq(mem, newval, oldval) \
  __v7_compare_and_exchange_bool_acq (mem, newval, oldval)

# define atomic_exchange_acq(mem, newval) \
  __v7_exchange_acq (mem, newval)

# define atomic_exchange_and_add(mem, value) \
  __v7_exchange_and_add (mem, value)

# define atomic_compare_and_exchange_val_24_acq(mem, newval, oldval) \
  ({								      \
     if (sizeof (*mem) != 4)					      \
       abort ();						      \
     __v7_compare_and_exchange_val_24_acq (mem, newval, oldval); })

# define atomic_exchange_24_rel(mem, newval) \
  ({								      \
     if (sizeof (*mem) != 4)					      \
       abort ();						      \
     __v7_exchange_24_rel (mem, newval); })

#else



/*
   Here's what we'd like to do:

   In libc.a/libpthread.a etc. we don't know if we'll be run on
   pre-v9 or v9 CPU.  To be interoperable with dynamically linked
   apps on v9 CPUs e.g. with process shared primitives, use cas insn
   on v9 CPUs and ldstub on pre-v9.

   However, we have no good way to test at run time that I know of,
   so resort to the lowest common denominator (v7 ops) -austinf
 */
#define __atomic_is_v9 0

# define atomic_compare_and_exchange_val_acq(mem, newval, oldval) \
  ({								      \
     __typeof (*mem) __acev_wret;				      \
     if (sizeof (*mem) != 4)					      \
       abort ();						      \
     if (__atomic_is_v9)					      \
       __acev_wret						      \
	 = __v9_compare_and_exchange_val_32_acq (mem, newval, oldval);\
     else							      \
       __acev_wret						      \
	 = __v7_compare_and_exchange_val_acq (mem, newval, oldval);   \
     __acev_wret; })

# define atomic_compare_and_exchange_bool_acq(mem, newval, oldval) \
  ({								      \
     int __acev_wret;						      \
     if (sizeof (*mem) != 4)					      \
       abort ();						      \
     if (__atomic_is_v9)					      \
       {							      \
	 __typeof (oldval) __acev_woldval = (oldval);		      \
	 __acev_wret						      \
	   = __v9_compare_and_exchange_val_32_acq (mem, newval,	      \
						   __acev_woldval)    \
	     != __acev_woldval;					      \
       }							      \
     else							      \
       __acev_wret						      \
	 = __v7_compare_and_exchange_bool_acq (mem, newval, oldval);  \
     __acev_wret; })

# define atomic_exchange_rel(mem, newval) \
  ({								      \
     __typeof (*mem) __acev_wret;				      \
     if (sizeof (*mem) != 4)					      \
       abort ();						      \
     if (__atomic_is_v9)					      \
       {							      \
	 __typeof (mem) __acev_wmemp = (mem);			      \
	 __typeof (*(mem)) __acev_wval = (newval);		      \
	 do							      \
	   __acev_wret = *__acev_wmemp;				      \
	 while (unlikely					      \
		  (__v9_compare_and_exchange_val_32_acq (__acev_wmemp,\
							 __acev_wval, \
							 __acev_wret) \
		   != __acev_wret));				      \
       }							      \
     else							      \
       __acev_wret = __v7_exchange_acq (mem, newval);		      \
     __acev_wret; })

# define atomic_compare_and_exchange_val_24_acq(mem, newval, oldval) \
  ({								      \
     __typeof (*mem) __acev_wret;				      \
     if (sizeof (*mem) != 4)					      \
       abort ();						      \
     if (__atomic_is_v9)					      \
       __acev_wret						      \
	 = __v9_compare_and_exchange_val_32_acq (mem, newval, oldval);\
     else							      \
       __acev_wret						      \
	 = __v7_compare_and_exchange_val_24_acq (mem, newval, oldval);\
     __acev_wret; })

# define atomic_exchange_24_rel(mem, newval) \
  ({								      \
     __typeof (*mem) __acev_w24ret;				      \
     if (sizeof (*mem) != 4)					      \
       abort ();						      \
     if (__atomic_is_v9)					      \
       __acev_w24ret = atomic_exchange_rel (mem, newval);	      \
     else							      \
       __acev_w24ret = __v7_exchange_24_rel (mem, newval);	      \
     __acev_w24ret; })

#endif

#endif	/* __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4 */

#endif	/* bits/atomic.h */
