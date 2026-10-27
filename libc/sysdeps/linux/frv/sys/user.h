/* Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.  */

#ifndef _SYS_USER_H
#define _SYS_USER_H	1

/* The register sets live in the kernel's uapi <asm/registers.h>; the
   core file layout follows arch/frv/include/asm/user.h.  */
#include <asm/registers.h>

struct user
{
  struct user_context regs;	/* registers, as ptrace returns them */
  unsigned long int u_tsize;	/* text segment size (pages) */
  unsigned long int u_dsize;	/* data segment size (pages) */
  unsigned long int u_ssize;	/* stack segment size (pages) */
  unsigned long int start_code;	/* start address of text */
  unsigned long int start_stack;	/* bottom of the stack */
  long int signal;		/* signal that caused the core dump */
  unsigned long int magic;	/* identifies a core file */
  char u_comm[32];		/* command that was responsible */
};

#define UPAGES			1
#define HOST_TEXT_START_ADDR	(u.start_code)
#define HOST_STACK_END_ADDR	(u.start_stack + u.u_ssize * NBPG)

#endif /* sys/user.h */
