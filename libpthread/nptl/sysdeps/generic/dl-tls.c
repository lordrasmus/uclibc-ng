/*
 * Thread-local storage for static programs: the dynamic linker's code in
 * ldso/ldso/dl-tls.c, on libc's functions instead of ld.so's own.
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#if defined SHARED || defined NOT_IN_libc
# error in buildsystem: This file is for libc.a
#endif

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ldso.h>

#undef _dl_malloc
#undef _dl_free
#undef _dl_memcpy
#undef _dl_memset
#undef _dl_dprintf
#undef _dl_exit
#define _dl_malloc	malloc
#define _dl_calloc	calloc
#define _dl_realloc	realloc
#define _dl_free	free
#define _dl_memcpy	memcpy
#define _dl_memset	memset
/* The messages are fixed strings: no stdio for them, and no exit
   handlers on the way out.  */
#define _dl_dprintf(fd, msg)	((void) write (fd, msg, sizeof (msg) - 1))
#define _dl_exit	_exit

extern dtv_t static_dtv;

#include "../../../../ldso/ldso/dl-tls.c"
