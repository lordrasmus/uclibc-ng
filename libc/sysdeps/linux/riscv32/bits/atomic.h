/* Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.  */

#include <bits/atomic-builtins.h>

/* pause from Zihintpause, encoded as a fence hint that cores without the
   extension execute as a nop.  */
#define atomic_delay() __asm__ __volatile__ (".4byte 0x0100000f")
