/* Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.  */

/* gcc emits only local-exec and initial-exec on cris, so a static
   program needs no __tls_get_addr.  */
#include <sysdeps/generic/libc-tls.c>
