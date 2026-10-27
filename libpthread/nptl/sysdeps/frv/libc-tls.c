/* Thread-local storage handling in the ELF dynamic linker.  FR-V version.
   Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.  */

/* FR-V code reaches dynamic TLS through TLS descriptors, never through
   __tls_get_addr, and the linker resolves them in a static link.  */
#include <sysdeps/generic/libc-tls.c>
