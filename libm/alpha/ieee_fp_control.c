/* Read and set the IEEE software completion control word of the kernel.
   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB in this tarball.  */

#include <fenv.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <asm/sysinfo.h>

unsigned long int
__ieee_get_fp_control (void)
{
  unsigned long int swcr = 0;

  syscall (__NR_osf_getsysinfo, GSI_IEEE_FP_CONTROL, &swcr, 0, 0, 0);
  return swcr;
}

void
__ieee_set_fp_control (unsigned long int swcr)
{
  syscall (__NR_osf_setsysinfo, SSI_IEEE_FP_CONTROL, &swcr, 0, 0, 0);
}
