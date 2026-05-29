/*
 * Copyright (C) 2016 Andes Technology, Inc.
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

/*
 * nds32 Linux syscall convention:
 *   - syscall number in $r15 *and* in the "syscall" SWID immediate
 *   - arguments       in $r0 .. $r5 (a 7th is spilled to the stack)
 *   - return value    in $r0
 *
 * The number is placed in both spots: mainline reads it from $r15, the legacy
 * Andes BSP reads the SWID, and gcc's unwinder expects SWID == nr. $r15 ($ta)
 * is the assembler temporary, so it is loaded inside the asm and clobbered
 * rather than bound to a register variable. A non-constant number (NCS) cannot
 * be an immediate and modern nds32 has no indirect syscall, so it travels in
 * $r15 only.
 */

#ifndef _BITS_SYSCALLS_H
#define _BITS_SYSCALLS_H
#ifndef _SYSCALL_H
# error "Never use <bits/syscalls.h> directly; include <sys/syscall.h> instead."
#endif

#ifndef __ASSEMBLER__
#include <errno.h>
#include <common/sysdep.h>

#define X(x) #x
#define Y(x) X(x)

#define __SYSCALL_CLOBBERS "$r15", "$lp", "memory"

#undef INTERNAL_SYSCALL_ERROR_P
#define INTERNAL_SYSCALL_ERROR_P(val, err) ((unsigned int) (val) >= 0xfffff001u)

#undef INTERNAL_SYSCALL_ERRNO
#define INTERNAL_SYSCALL_ERRNO(val, err)	(-(val))

#undef INLINE_SYSCALL
#define INLINE_SYSCALL(name, nr, args...)				\
  ({									\
     INTERNAL_SYSCALL_DECL (err);					\
     long result_var = INTERNAL_SYSCALL (name, err, nr, args);		\
     if (INTERNAL_SYSCALL_ERROR_P (result_var, err))			\
       {								\
         __set_errno (INTERNAL_SYSCALL_ERRNO (result_var, err));	\
         result_var = -1 ;						\
       }								\
     result_var;							\
  })

#undef INTERNAL_SYSCALL_DECL
#define INTERNAL_SYSCALL_DECL(err) do { } while (0)

#undef INTERNAL_SYSCALL
#define INTERNAL_SYSCALL(name, err, nr, args...) internal_syscall##nr(__NR_##name, err, args)

#undef INTERNAL_SYSCALL_NCS
#define INTERNAL_SYSCALL_NCS(name, err, nr, args...) internal_syscall_ncs##nr(name, err, args)

/* Constant number: emit it as the SWID immediate and into $r15. */

#define internal_syscall0(name, err, dummy...)				\
  ({									\
       register long ___res  __asm__("$r0");				\
       __asm__ volatile (						\
       "movi $r15, " Y(name) "\n\t"					\
       "syscall " Y(name) "\n\t"					\
       : "=r" (___res)							\
       :								\
       : __SYSCALL_CLOBBERS);						\
       ___res;								\
  })

#define internal_syscall1(name, err, arg1)				\
  ({									\
       register long ___res  __asm__("$r0");				\
       register long __arg1  __asm__("$r0") = (long) (arg1);		\
       __asm__ volatile (						\
       "movi $r15, " Y(name) "\n\t"					\
       "syscall " Y(name) "\n\t"					\
       : "=r" (___res)							\
       : "r" (__arg1)							\
       : __SYSCALL_CLOBBERS);						\
       ___res;								\
  })

#define internal_syscall2(name, err, arg1, arg2)			\
  ({									\
       register long ___res  __asm__("$r0");				\
       register long __arg1  __asm__("$r0") = (long) (arg1);		\
       register long __arg2  __asm__("$r1") = (long) (arg2);		\
       __asm__ volatile (						\
       "movi $r15, " Y(name) "\n\t"					\
       "syscall " Y(name) "\n\t"					\
       : "=r" (___res)							\
       : "r" (__arg1), "r" (__arg2)					\
       : __SYSCALL_CLOBBERS);						\
       ___res;								\
  })

#define internal_syscall3(name, err, arg1, arg2, arg3)			\
  ({									\
       register long ___res  __asm__("$r0");				\
       register long __arg1  __asm__("$r0") = (long) (arg1);		\
       register long __arg2  __asm__("$r1") = (long) (arg2);		\
       register long __arg3  __asm__("$r2") = (long) (arg3);		\
       __asm__ volatile (						\
       "movi $r15, " Y(name) "\n\t"					\
       "syscall " Y(name) "\n\t"					\
       : "=r" (___res)							\
       : "r" (__arg1), "r" (__arg2), "r" (__arg3)			\
       : __SYSCALL_CLOBBERS);						\
       ___res;								\
  })

#define internal_syscall4(name, err, arg1, arg2, arg3, arg4)		\
  ({									\
       register long ___res  __asm__("$r0");				\
       register long __arg1  __asm__("$r0") = (long) (arg1);		\
       register long __arg2  __asm__("$r1") = (long) (arg2);		\
       register long __arg3  __asm__("$r2") = (long) (arg3);		\
       register long __arg4  __asm__("$r3") = (long) (arg4);		\
       __asm__ volatile (						\
       "movi $r15, " Y(name) "\n\t"					\
       "syscall " Y(name) "\n\t"					\
       : "=r" (___res)							\
       : "r" (__arg1), "r" (__arg2), "r" (__arg3), "r" (__arg4)		\
       : __SYSCALL_CLOBBERS);						\
       ___res;								\
  })

#define internal_syscall5(name, err, arg1, arg2, arg3, arg4, arg5)	\
  ({									\
       register long ___res  __asm__("$r0");				\
       register long __arg1  __asm__("$r0") = (long) (arg1);		\
       register long __arg2  __asm__("$r1") = (long) (arg2);		\
       register long __arg3  __asm__("$r2") = (long) (arg3);		\
       register long __arg4  __asm__("$r3") = (long) (arg4);		\
       register long __arg5  __asm__("$r4") = (long) (arg5);		\
       __asm__ volatile (						\
       "movi $r15, " Y(name) "\n\t"					\
       "syscall " Y(name) "\n\t"					\
       : "=r" (___res)							\
       : "r" (__arg1), "r" (__arg2), "r" (__arg3), "r" (__arg4),		\
         "r" (__arg5)							\
       : __SYSCALL_CLOBBERS);						\
       ___res;								\
  })

#define internal_syscall6(name, err, arg1, arg2, arg3, arg4, arg5, arg6) \
  ({									\
       register long ___res  __asm__("$r0");				\
       register long __arg1  __asm__("$r0") = (long) (arg1);		\
       register long __arg2  __asm__("$r1") = (long) (arg2);		\
       register long __arg3  __asm__("$r2") = (long) (arg3);		\
       register long __arg4  __asm__("$r3") = (long) (arg4);		\
       register long __arg5  __asm__("$r4") = (long) (arg5);		\
       register long __arg6  __asm__("$r5") = (long) (arg6);		\
       __asm__ volatile (						\
       "movi $r15, " Y(name) "\n\t"					\
       "syscall " Y(name) "\n\t"					\
       : "=r" (___res)							\
       : "r" (__arg1), "r" (__arg2), "r" (__arg3), "r" (__arg4),		\
         "r" (__arg5), "r" (__arg6)					\
       : __SYSCALL_CLOBBERS);						\
       ___res;								\
  })

/* nds32 passes at most 6 syscall arguments in registers; a 7th is spilled
   to the stack. */
#define internal_syscall7(name, err, arg1, arg2, arg3, arg4, arg5, arg6, arg7) \
  ({									\
       register long ___res  __asm__("$r0");				\
       register long __arg1  __asm__("$r0") = (long) (arg1);		\
       register long __arg2  __asm__("$r1") = (long) (arg2);		\
       register long __arg3  __asm__("$r2") = (long) (arg3);		\
       register long __arg4  __asm__("$r3") = (long) (arg4);		\
       register long __arg5  __asm__("$r4") = (long) (arg5);		\
       register long __arg6  __asm__("$r5") = (long) (arg6);		\
       __asm__ volatile (						\
        "movi $r15, " Y(name) "\n\t"					\
        "addi10.sp\t #-4\n\t"						\
        CFI_ADJUST_CFA_OFFSET(4)"\n\t"					\
        "push\t %[a7]\n\t"						\
        CFI_ADJUST_CFA_OFFSET(4)"\n\t"					\
        "syscall " Y(name) "\n\t"					\
        "addi10.sp\t #4\n\t"						\
        CFI_ADJUST_CFA_OFFSET(-4)"\n\t"					\
        "pop\t %[a7]\n\t"						\
        CFI_ADJUST_CFA_OFFSET(-4)"\n\t"					\
       : "=r" (___res)							\
       : "r" (__arg1), "r" (__arg2), "r" (__arg3), "r" (__arg4),		\
         "r" (__arg5), "r" (__arg6), [a7] "r" ((long) (arg7))		\
       : __SYSCALL_CLOBBERS);						\
       ___res;								\
  })

/* Non-constant number: it cannot be a SWID immediate, so pass it in $r15
   only (mainline; modern nds32 has no indirect syscall). */

#define internal_syscall_ncs0(name, err, dummy...)			\
  ({									\
       register long ___res  __asm__("$r0");				\
       __asm__ volatile (						\
       "move $r15, %[nr]\n\t"						\
       "syscall 0x0\n\t"						\
       : "=r" (___res)							\
       : [nr] "r" ((long) (name))					\
       : __SYSCALL_CLOBBERS);						\
       ___res;								\
  })

#define internal_syscall_ncs1(name, err, arg1)				\
  ({									\
       register long ___res  __asm__("$r0");				\
       register long __arg1  __asm__("$r0") = (long) (arg1);		\
       __asm__ volatile (						\
       "move $r15, %[nr]\n\t"						\
       "syscall 0x0\n\t"						\
       : "=r" (___res)							\
       : [nr] "r" ((long) (name)), "r" (__arg1)				\
       : __SYSCALL_CLOBBERS);						\
       ___res;								\
  })

#define internal_syscall_ncs2(name, err, arg1, arg2)			\
  ({									\
       register long ___res  __asm__("$r0");				\
       register long __arg1  __asm__("$r0") = (long) (arg1);		\
       register long __arg2  __asm__("$r1") = (long) (arg2);		\
       __asm__ volatile (						\
       "move $r15, %[nr]\n\t"						\
       "syscall 0x0\n\t"						\
       : "=r" (___res)							\
       : [nr] "r" ((long) (name)), "r" (__arg1), "r" (__arg2)		\
       : __SYSCALL_CLOBBERS);						\
       ___res;								\
  })

#define internal_syscall_ncs3(name, err, arg1, arg2, arg3)		\
  ({									\
       register long ___res  __asm__("$r0");				\
       register long __arg1  __asm__("$r0") = (long) (arg1);		\
       register long __arg2  __asm__("$r1") = (long) (arg2);		\
       register long __arg3  __asm__("$r2") = (long) (arg3);		\
       __asm__ volatile (						\
       "move $r15, %[nr]\n\t"						\
       "syscall 0x0\n\t"						\
       : "=r" (___res)							\
       : [nr] "r" ((long) (name)), "r" (__arg1), "r" (__arg2),		\
         "r" (__arg3)							\
       : __SYSCALL_CLOBBERS);						\
       ___res;								\
  })

#define internal_syscall_ncs4(name, err, arg1, arg2, arg3, arg4)	\
  ({									\
       register long ___res  __asm__("$r0");				\
       register long __arg1  __asm__("$r0") = (long) (arg1);		\
       register long __arg2  __asm__("$r1") = (long) (arg2);		\
       register long __arg3  __asm__("$r2") = (long) (arg3);		\
       register long __arg4  __asm__("$r3") = (long) (arg4);		\
       __asm__ volatile (						\
       "move $r15, %[nr]\n\t"						\
       "syscall 0x0\n\t"						\
       : "=r" (___res)							\
       : [nr] "r" ((long) (name)), "r" (__arg1), "r" (__arg2),		\
         "r" (__arg3), "r" (__arg4)					\
       : __SYSCALL_CLOBBERS);						\
       ___res;								\
  })

#define internal_syscall_ncs5(name, err, arg1, arg2, arg3, arg4, arg5)	\
  ({									\
       register long ___res  __asm__("$r0");				\
       register long __arg1  __asm__("$r0") = (long) (arg1);		\
       register long __arg2  __asm__("$r1") = (long) (arg2);		\
       register long __arg3  __asm__("$r2") = (long) (arg3);		\
       register long __arg4  __asm__("$r3") = (long) (arg4);		\
       register long __arg5  __asm__("$r4") = (long) (arg5);		\
       __asm__ volatile (						\
       "move $r15, %[nr]\n\t"						\
       "syscall 0x0\n\t"						\
       : "=r" (___res)							\
       : [nr] "r" ((long) (name)), "r" (__arg1), "r" (__arg2),		\
         "r" (__arg3), "r" (__arg4), "r" (__arg5)			\
       : __SYSCALL_CLOBBERS);						\
       ___res;								\
  })

#define internal_syscall_ncs6(name, err, arg1, arg2, arg3, arg4, arg5, arg6) \
  ({									\
       register long ___res  __asm__("$r0");				\
       register long __arg1  __asm__("$r0") = (long) (arg1);		\
       register long __arg2  __asm__("$r1") = (long) (arg2);		\
       register long __arg3  __asm__("$r2") = (long) (arg3);		\
       register long __arg4  __asm__("$r3") = (long) (arg4);		\
       register long __arg5  __asm__("$r4") = (long) (arg5);		\
       register long __arg6  __asm__("$r5") = (long) (arg6);		\
       __asm__ volatile (						\
       "move $r15, %[nr]\n\t"						\
       "syscall 0x0\n\t"						\
       : "=r" (___res)							\
       : [nr] "r" ((long) (name)), "r" (__arg1), "r" (__arg2),		\
         "r" (__arg3), "r" (__arg4), "r" (__arg5), "r" (__arg6)		\
       : __SYSCALL_CLOBBERS);						\
       ___res;								\
  })

#endif /* ! __ASSEMBLER__  */
#endif /* _BITS_SYSCALLS_H */
