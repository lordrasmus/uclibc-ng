/* Thread-local storage handling in the ELF dynamic linker.  hppa version.
   Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.  */

#include <sysdeps/generic/libc-tls.c>
#include <dl-tls.h>

#if USE_TLS

/* The hppa linker does not turn general-dynamic accesses into local-exec
   ones, so __tls_get_addr is called in static programs too.  The module
   is then always 1.  */

void *
__tls_get_addr (tls_index *ti)
{
  dtv_t *dtv = THREAD_DTV ();
  return (char *) dtv[1].pointer.val + ti->ti_offset;
}

#endif
