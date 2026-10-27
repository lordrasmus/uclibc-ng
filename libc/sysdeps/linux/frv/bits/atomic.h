/* Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.  */

#ifndef _FRV_BITS_ATOMIC_H
#define _FRV_BITS_ATOMIC_H	1

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

/* FR-V Linux is uniprocessor only.  */
#define atomic_full_barrier()	__asm__ __volatile__ ("" ::: "memory")
#define atomic_read_barrier()	atomic_full_barrier ()
#define atomic_write_barrier()	atomic_full_barrier ()

/* FR-V has no compare-and-swap instruction, and the kernel's CC3 trick does
   not work in userspace: only entries from kernel mode clear CC3.  The
   kernel provides traps for userspace instead (arch/frv/kernel/traps.c):
   tira gr0,#120 is cmpxchg (gr8 = mem, gr9 = old, gr10 = new), #121 is
   xchg (gr8 = mem, gr9 = new); both return the previous value in gr5.  */
#define __arch_compare_and_exchange_val_32_acq(mem, newval, oldval) \
  ({ register uint32_t *__gr8 __asm__ ("gr8") = (uint32_t *) (mem);	\
     register uint32_t __gr9 __asm__ ("gr9") = (uint32_t) (oldval);	\
     register uint32_t __gr10 __asm__ ("gr10") = (uint32_t) (newval);	\
     register uint32_t __gr5 __asm__ ("gr5");				\
     __asm__ __volatile__ ("tira gr0,#120"				\
		       : "=r" (__gr5), "+r" (__gr9), "+m" (*__gr8)	\
		       : "r" (__gr8), "r" (__gr10)			\
		       : "memory");					\
     (__typeof (*(mem))) __gr5; })

#define atomic_exchange_acq(mem, newvalue) \
  ({ __typeof (*(mem)) __result;					\
     if (sizeof (*(mem)) != 4)						\
       abort ();							\
     else								\
       {								\
	 register uint32_t *__gr8 __asm__ ("gr8") = (uint32_t *) (mem);	\
	 register uint32_t __gr9 __asm__ ("gr9") = (uint32_t) (newvalue); \
	 register uint32_t __gr5 __asm__ ("gr5");			\
	 __asm__ __volatile__ ("tira gr0,#121"				\
			   : "=r" (__gr5), "+r" (__gr9), "+m" (*__gr8)	\
			   : "r" (__gr8)				\
			   : "memory");					\
	 __result = (__typeof (*(mem))) __gr5;				\
       }								\
     __result; })

/* The traps are 32-bit only.  uClibc's atomic users are int- and
   pointer-sized, so these stay non-atomic rather than have a 32-bit access
   clobber the neighbouring bytes.  */
#define __arch_compare_and_exchange_val_8_acq(mem, newval, oldval) \
  ({ __typeof (mem) __gmem = (mem);					\
     __typeof (*(mem)) __gret = *__gmem;				\
     if (__gret == (oldval))						\
       *__gmem = (newval);						\
     __gret; })

#define __arch_compare_and_exchange_val_16_acq(mem, newval, oldval) \
  __arch_compare_and_exchange_val_8_acq (mem, newval, oldval)

#define __arch_compare_and_exchange_val_64_acq(mem, newval, oldval) \
  ({ __typeof (*(mem)) __ret = *(mem); abort (); (void) (newval);	\
     (void) (oldval); __ret; })

#endif /* _FRV_BITS_ATOMIC_H */
