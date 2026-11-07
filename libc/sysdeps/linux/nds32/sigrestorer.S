/*
 * Signal return trampolines for the Andes BSP kernel (Linux 3.4).
 *
 * Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
 * in this tarball.
 */

#include <sys/syscall.h>

#ifdef __NR_SYSCALL_BASE

/* CFA is $sp at the trampoline, i.e. the signal frame.  SC is the offset
   of its struct sigcontext: 0 in struct sigframe; 156 in struct
   rt_sigframe, behind pinfo, puc, siginfo (128) and uc_flags, uc_link,
   uc_stack.  Column 32 is the interrupted pc ($ipc). */
#define SIGFRAME_CFI(SC)					\
	.cfi_signal_frame;					\
	.cfi_return_column 32;					\
	.cfi_def_cfa 31, 0;					\
	.cfi_offset 0, SC + 12;   .cfi_offset 1, SC + 16;	\
	.cfi_offset 2, SC + 20;   .cfi_offset 3, SC + 24;	\
	.cfi_offset 4, SC + 28;   .cfi_offset 5, SC + 32;	\
	.cfi_offset 6, SC + 36;   .cfi_offset 7, SC + 40;	\
	.cfi_offset 8, SC + 44;   .cfi_offset 9, SC + 48;	\
	.cfi_offset 10, SC + 52;  .cfi_offset 11, SC + 56;	\
	.cfi_offset 12, SC + 60;  .cfi_offset 13, SC + 64;	\
	.cfi_offset 14, SC + 68;  .cfi_offset 15, SC + 72;	\
	.cfi_offset 16, SC + 76;  .cfi_offset 17, SC + 80;	\
	.cfi_offset 18, SC + 84;  .cfi_offset 19, SC + 88;	\
	.cfi_offset 20, SC + 92;  .cfi_offset 21, SC + 96;	\
	.cfi_offset 22, SC + 100; .cfi_offset 23, SC + 104;	\
	.cfi_offset 24, SC + 108; .cfi_offset 25, SC + 112;	\
	.cfi_offset 28, SC + 116; .cfi_offset 29, SC + 120;	\
	.cfi_offset 30, SC + 124; .cfi_offset 31, SC + 128;	\
	.cfi_offset 32, SC + 132

	.text

/* The unwinder looks up the return address minus one: the FDE starts
   one instruction early. */
	.align	2
	.cfi_startproc
	SIGFRAME_CFI(0)
	nop
	.align	2
	.globl	__default_sa_restorer
	.hidden	__default_sa_restorer
	.type	__default_sa_restorer, @function
__default_sa_restorer:
	syscall	__NR_sigreturn
	.cfi_endproc
	.size	__default_sa_restorer, .-__default_sa_restorer

	.align	2
	.cfi_startproc
	SIGFRAME_CFI(156)
	nop
	.align	2
	.globl	__default_rt_sa_restorer
	.hidden	__default_rt_sa_restorer
	.type	__default_rt_sa_restorer, @function
__default_rt_sa_restorer:
	syscall	__NR_rt_sigreturn
	.cfi_endproc
	.size	__default_rt_sa_restorer, .-__default_rt_sa_restorer

#endif
