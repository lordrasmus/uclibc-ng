#ifndef _BITS_SYSCALLS_H
#define _BITS_SYSCALLS_H
#ifndef _SYSCALL_H
# error "Never use <bits/syscalls.h> directly; include <sys/syscall.h> instead."
#endif


#ifndef __ASSEMBLER__

/*
 * No local register variables for the arguments: without a barrel
 * shifter gcc turns shifts into libcalls (__ashrdi3, __ashrsi3) only when
 * it expands to RTL, and such a call clobbers r1-r10 under a register
 * variable that is already set.  lseek64 handed the kernel the result of
 * "offset >> 32" as fd that way.  The arguments go in as plain operands,
 * the asm moves them to r1-r6 itself, and r1-r8 are clobbers so that gcc
 * puts no operand there.
 */
#define INTERNAL_SYSCALL_NCS(name, err, nr, args...)			\
	({								\
		register int __ret;					\
		LOAD_ARGS_##nr (args);					\
		__asm__ __volatile__(					\
			MOVE_ARGS_##nr					\
			"mv	r8, %1\n\t"				\
			"scall\n\t"					\
			"mv	%0, r1"					\
			: "=r" (__ret)					\
			: "r" ((int) (name)) ASM_ARGS_##nr		\
			: __SYSCALL_CLOBBERS );				\
		__ret;							\
	})

#define LOAD_ARGS_0()
#define MOVE_ARGS_0
#define ASM_ARGS_0
#define LOAD_ARGS_1(a1)							\
	int _a1 = (int) (a1)
#define MOVE_ARGS_1	MOVE_ARGS_0 "mv	r1, %2\n\t"
#define ASM_ARGS_1	ASM_ARGS_0, "r" (_a1)
#define LOAD_ARGS_2(a1, a2)						\
	LOAD_ARGS_1 (a1);						\
	int _a2 = (int) (a2)
#define MOVE_ARGS_2	MOVE_ARGS_1 "mv	r2, %3\n\t"
#define ASM_ARGS_2	ASM_ARGS_1, "r" (_a2)
#define LOAD_ARGS_3(a1, a2, a3)						\
	LOAD_ARGS_2 (a1, a2);						\
	int _a3 = (int) (a3)
#define MOVE_ARGS_3	MOVE_ARGS_2 "mv	r3, %4\n\t"
#define ASM_ARGS_3	ASM_ARGS_2, "r" (_a3)
#define LOAD_ARGS_4(a1, a2, a3, a4)					\
	LOAD_ARGS_3 (a1, a2, a3);					\
	int _a4 = (int) (a4)
#define MOVE_ARGS_4	MOVE_ARGS_3 "mv	r4, %5\n\t"
#define ASM_ARGS_4	ASM_ARGS_3, "r" (_a4)
#define LOAD_ARGS_5(a1, a2, a3, a4, a5)					\
	LOAD_ARGS_4 (a1, a2, a3, a4);					\
	int _a5 = (int) (a5)
#define MOVE_ARGS_5	MOVE_ARGS_4 "mv	r5, %6\n\t"
#define ASM_ARGS_5	ASM_ARGS_4, "r" (_a5)
#define LOAD_ARGS_6(a1, a2, a3, a4, a5, a6)				\
	LOAD_ARGS_5 (a1, a2, a3, a4, a5);				\
	int _a6 = (int) (a6)
#define MOVE_ARGS_6	MOVE_ARGS_5 "mv	r6, %7\n\t"
#define ASM_ARGS_6	ASM_ARGS_5, "r" (_a6)

/* The kernel restores every register but r1. */
#define __SYSCALL_CLOBBERS "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8", "memory"

#endif /* __ASSEMBLER__ */
#endif /* _BITS_SYSCALLS_H */
