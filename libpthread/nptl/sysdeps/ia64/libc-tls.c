/* Thread-local storage handling in a static program.  IA-64 version.
   Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.  */

#include <sysdeps/generic/libc-tls.c>
#include <dl-tls.h>

#if USE_TLS
/* The linker does not turn every general-dynamic access into a static
   one, so __tls_get_addr is called in static programs too.  The module
   is then always 1.  */
void *
__tls_get_addr (size_t m, size_t offset)
{
  dtv_t *dtv = THREAD_DTV ();
  return (char *) dtv[1].pointer.val + offset;
}
#endif
