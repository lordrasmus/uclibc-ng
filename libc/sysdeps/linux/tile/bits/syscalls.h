/* Copyright (C) 2011-2018 Free Software Foundation, Inc.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library.  If not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _BITS_SYSCALLS_H
#define _BITS_SYSCALLS_H
#ifndef _SYSCALL_H
# error "Never use <bits/syscalls.h> directly; include <sys/syscall.h> instead."
#endif

/* The kernel leaves -errno in r0 and copies the positive errno to r1;
   r0 alone is enough.  */
#undef INTERNAL_SYSCALL_NCS
#define INTERNAL_SYSCALL_NCS(number, err, nr, args...)  \
  internal_syscall##nr (number, args)

#define internal_syscall0(num, dummy...)                           \
  ({                                                                    \
    long _sys_result, __SYSCALL_CLOBBER_DECLS;                          \
    __asm__ __volatile__ (                                              \
      "swint1"                                                          \
      : "=R00" (_sys_result), "=R01" (_clobber_r1), __SYSCALL_CLOBBER_OUTPUTS   \
      : "R10" (num)                                                     \
      : __SYSCALL_CLOBBERS);                                            \
    _sys_result;                                                        \
  })

#define internal_syscall1(num, arg0)                               \
  ({                                                                    \
    long _sys_result, __SYSCALL_CLOBBER_DECLS;                          \
    __asm__ __volatile__ (                                              \
      "swint1"                                                          \
      : "=R00" (_sys_result), "=R01" (_clobber_r1), __SYSCALL_CLOBBER_OUTPUTS   \
      : "R10" (num), "R00" (arg0)                                       \
      : __SYSCALL_CLOBBERS);                                            \
    _sys_result;                                                        \
  })

#define internal_syscall2(num, arg0, arg1)                         \
  ({                                                                    \
    long _sys_result, __SYSCALL_CLOBBER_DECLS;                          \
    __asm__ __volatile__ (                                              \
      "swint1"                                                          \
      : "=R00" (_sys_result), "=R01" (_clobber_r1), __SYSCALL_CLOBBER_OUTPUTS   \
      : "R10" (num), "R00" (arg0), "R01" (arg1)                         \
      : __SYSCALL_CLOBBERS);                                            \
    _sys_result;                                                        \
  })

#define internal_syscall3(num, arg0, arg1, arg2)                   \
  ({                                                                    \
    long _sys_result, __SYSCALL_CLOBBER_DECLS;                          \
    __asm__ __volatile__ (                                              \
      "swint1"                                                          \
      : "=R00" (_sys_result), "=R01" (_clobber_r1), __SYSCALL_CLOBBER_OUTPUTS   \
      : "R10" (num), "R00" (arg0), "R01" (arg1), "R02" (arg2)           \
      : __SYSCALL_CLOBBERS);                                            \
    _sys_result;                                                        \
  })

#define internal_syscall4(num, arg0, arg1, arg2, arg3)             \
  ({                                                                    \
    long _sys_result, __SYSCALL_CLOBBER_DECLS;                          \
    __asm__ __volatile__ (                                              \
      "swint1"                                                          \
      : "=R00" (_sys_result), "=R01" (_clobber_r1), __SYSCALL_CLOBBER_OUTPUTS   \
      : "R10" (num), "R00" (arg0), "R01" (arg1), "R02" (arg2),          \
        "R03" (arg3)                                                    \
      : __SYSCALL_CLOBBERS);                                            \
    _sys_result;                                                        \
  })

#define internal_syscall5(num, arg0, arg1, arg2, arg3, arg4)       \
  ({                                                                    \
    long _sys_result, __SYSCALL_CLOBBER_DECLS;                          \
    __asm__ __volatile__ (                                              \
      "swint1"                                                          \
      : "=R00" (_sys_result), "=R01" (_clobber_r1), __SYSCALL_CLOBBER_OUTPUTS   \
      : "R10" (num), "R00" (arg0), "R01" (arg1), "R02" (arg2),          \
        "R03" (arg3), "R04" (arg4)                                      \
      : __SYSCALL_CLOBBERS);                                            \
    _sys_result;                                                        \
  })

#define internal_syscall6(num, arg0, arg1, arg2, arg3, arg4, arg5) \
  ({                                                                    \
    long _sys_result, __SYSCALL_CLOBBER_DECLS;                          \
    __asm__ __volatile__ (                                              \
      "swint1"                                                          \
      : "=R00" (_sys_result), "=R01" (_clobber_r1), __SYSCALL_CLOBBER_OUTPUTS   \
      : "R10" (num), "R00" (arg0), "R01" (arg1), "R02" (arg2),          \
        "R03" (arg3), "R04" (arg4), "R05" (arg5)                        \
      : __SYSCALL_CLOBBERS);                                            \
    _sys_result;                                                        \
  })

#undef __SYSCALL_CLOBBERS
#define __SYSCALL_CLOBBERS                                      \
  "r6",  "r7",                                                  \
    "r8",  "r9",        "r11", "r12", "r13", "r14", "r15",      \
    "r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23",     \
    "r24", "r25", "r26", "r27", "r28", "r29", "memory"

/* gcc doesn't seem to allow an input operand to be clobbered, so we
   fake it with dummy outputs. */
#define __SYSCALL_CLOBBER_DECLS                                         \
  _clobber_r1, _clobber_r2, _clobber_r3, _clobber_r4, _clobber_r5, _clobber_r10

#define __SYSCALL_CLOBBER_OUTPUTS                                       \
  "=R02" (_clobber_r2), "=R03" (_clobber_r3), "=R04" (_clobber_r4),     \
    "=R05" (_clobber_r5), "=R10" (_clobber_r10)

#endif
