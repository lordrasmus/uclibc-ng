/* Raise given exceptions.
   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB in this tarball.  */

#include <fenv.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <asm/sysinfo.h>

int
feraiseexcept (int excepts)
{
  unsigned long int exc = excepts;

  /* The kernel sets the status bits and sends SIGFPE where the trap is
     enabled.  */
  return syscall (__NR_osf_setsysinfo, SSI_IEEE_RAISE_EXCEPTION, &exc,
		  0, 0, 0) != 0;
}
