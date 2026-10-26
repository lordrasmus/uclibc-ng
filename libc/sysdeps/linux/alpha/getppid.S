/* getppid for alpha.
   Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.  */

#include <features.h>
#include <sys/syscall.h>
#include <sys/regdef.h>
#include <asm/pal.h>

/* getxpid returns the pid in v0 and the parent's pid in a4.  It is in
   every kernel; the getppid syscall came to alpha only later.  */

.globl getppid
.align 4
.ent getppid, 0
getppid:
	.frame sp, 0, ra
	.prologue 0
	lda	v0, __NR_getxpid
	call_pal PAL_callsys
	mov	a4, v0
	ret

.end getppid
