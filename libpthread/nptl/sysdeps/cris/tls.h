/* Definitions for thread-local data handling.  NPTL/CRIS version.
   Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.

   TLS variant II: the thread descriptor is at the thread pointer, the
   static TLS blocks below it.  CRIS v32 keeps the thread pointer in the
   upper 24 bits of $pid, as binutils implements it; v10 has no register
   for it, so gcc reserves r6.  */

#ifndef _TLS_H
#define _TLS_H

#ifndef __ASSEMBLER__
# include <stdbool.h>
# include <stddef.h>
# include <stdint.h>
# include <stdlib.h>
# include <list.h>
# include <bits/kernel-features.h>

/* Type for the dtv.  */
typedef union dtv
{
  size_t counter;
  struct
  {
    void *val;
    bool is_static;
  } pointer;
} dtv_t;

typedef struct
{
  void *tcb;		/* Pointer to the TCB.  Not necessary the
			   thread descriptor used by libpthread.  */
  dtv_t *dtv;
  void *self;
  int multiple_threads;
  uintptr_t sysinfo;
  uintptr_t stack_guard;
  uintptr_t pointer_guard;
  int gscope_flag;
#ifndef __ASSUME_PRIVATE_FUTEX
  int private_futex;
#endif
} tcbhead_t;

#else /* __ASSEMBLER__ */
# include <tcb-offsets.h>
#endif /* __ASSEMBLER__ */

/* We require TLS support in the tools.  */
#define HAVE_TLS_SUPPORT
#define HAVE___THREAD 1
#define HAVE_TLS_MODEL_ATTRIBUTE 1

/* Signal that TLS support is available.  */
#define USE_TLS        1

#ifndef __ASSEMBLER__
/* Get system call information.  */
# include <sysdep.h>

/* Get the thread descriptor definition.  */
# include <descr.h>

# ifdef __arch_v32
/* The low 8 bits of $pid are the MMU context.  */
#  define READ_THREAD_POINTER() \
  ({ unsigned long __tp;						\
     __asm__ __volatile__ ("move $pid,%0" : "=r" (__tp));		\
     (void *) (__tp & ~0xffUL); })
# else
#  define READ_THREAD_POINTER() \
  ({ void *__tp;							\
     __asm__ __volatile__ ("move.d $r6,%0" : "=r" (__tp));		\
     __tp; })
# endif

/* This is the size of the initial TCB.  Can't be just sizeof (tcbhead_t),
   because NPTL getpid, __libc_alloca_cutoff etc. need (almost) the whole
   struct pthread even when not linked with -lpthread.  */
# define TLS_INIT_TCB_SIZE sizeof (struct pthread)

/* Alignment requirements for the initial TCB: on v32 the thread pointer
   has no room for the low 8 bits.  */
# define TLS_INIT_TCB_ALIGN 256

/* This is the size of the TCB.  */
# define TLS_TCB_SIZE sizeof (struct pthread)

/* Alignment requirements for the TCB.  */
# define TLS_TCB_ALIGN 256

/* The TCB can have any size and the memory following the address the
   thread pointer points to is unspecified.  Allocate the TCB there.  */
# define TLS_TCB_AT_TP	1

/* Install the dtv pointer.  The pointer passed is to the element with
   index -1 which contain the length.  */
# define INSTALL_DTV(descr, dtvp) \
  ((tcbhead_t *) (descr))->dtv = (dtvp) + 1

/* Install new dtv for current thread.  */
# define INSTALL_NEW_DTV(DTV) \
  (((tcbhead_t *) READ_THREAD_POINTER ())->dtv = (DTV))

/* Return dtv of given thread descriptor.  */
# define GET_DTV(descr) \
  (((tcbhead_t *) (descr))->dtv)

/* Code to initially initialize the thread pointer.  */
# define TLS_INIT_TP(descr, secondcall) \
  ({ INTERNAL_SYSCALL_DECL (__err);					\
     long __r = INTERNAL_SYSCALL (set_thread_area, __err, 1, (descr));	\
     INTERNAL_SYSCALL_ERROR_P (__r, __err)				\
       ? "set_thread_area failed when setting up thread-local storage\n" \
       : NULL; })

/* Return the address of the dtv for the current thread.  */
# define THREAD_DTV() \
  (((tcbhead_t *) READ_THREAD_POINTER ())->dtv)

/* Return the thread descriptor for the current thread.  */
# define THREAD_SELF \
  ((struct pthread *) READ_THREAD_POINTER ())

/* Magic for libthread_db to know how to do THREAD_SELF: the thread
   pointer is the descriptor, and $pid is not a plain register value.  */
# define DB_THREAD_SELF \
  CONST_THREAD_AREA (32, 0)

/* Access to data in the thread descriptor is easy.  */
#define THREAD_GETMEM(descr, member) \
  descr->member
#define THREAD_GETMEM_NC(descr, member, idx) \
  descr->member[idx]
#define THREAD_SETMEM(descr, member, value) \
  descr->member = (value)
#define THREAD_SETMEM_NC(descr, member, idx, value) \
  descr->member[idx] = (value)

/* Set the stack guard field in TCB head.  */
#define THREAD_SET_STACK_GUARD(value) \
  THREAD_SETMEM (THREAD_SELF, header.stack_guard, value)
# define THREAD_COPY_STACK_GUARD(descr) \
  ((descr)->header.stack_guard \
   = THREAD_GETMEM (THREAD_SELF, header.stack_guard))

/* Get/set the stack guard field in TCB head.  */
#define THREAD_GET_POINTER_GUARD() \
  THREAD_GETMEM (THREAD_SELF, header.pointer_guard)
#define THREAD_SET_POINTER_GUARD(value) \
  THREAD_SETMEM (THREAD_SELF, header.pointer_guard, value)
# define THREAD_COPY_POINTER_GUARD(descr) \
  ((descr)->header.pointer_guard = THREAD_GET_POINTER_GUARD ())

/* Get and set the global scope generation counter in struct pthread.  */
#define THREAD_GSCOPE_FLAG_UNUSED 0
#define THREAD_GSCOPE_FLAG_USED   1
#define THREAD_GSCOPE_FLAG_WAIT   2
#define THREAD_GSCOPE_RESET_FLAG() \
  do									     \
    { int __res								     \
	= atomic_exchange_rel (&THREAD_SELF->header.gscope_flag,	     \
			       THREAD_GSCOPE_FLAG_UNUSED);		     \
      if (__res == THREAD_GSCOPE_FLAG_WAIT)				     \
	lll_futex_wake (&THREAD_SELF->header.gscope_flag, 1, LLL_PRIVATE);   \
    }									     \
  while (0)
#define THREAD_GSCOPE_SET_FLAG() \
  do									     \
    {									     \
      THREAD_SELF->header.gscope_flag = THREAD_GSCOPE_FLAG_USED;	     \
      atomic_write_barrier ();						     \
    }									     \
  while (0)
#define THREAD_GSCOPE_WAIT() \
  GL(dl_wait_lookup_done) ()

#endif /* !ASSEMBLER */

#endif	/* tls.h */
