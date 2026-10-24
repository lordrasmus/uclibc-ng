/*
 * strerror_l, POSIX.1-2008.
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <features.h>
#include <string.h>
#include "_syserrmsg.h"

/* The messages exist in one language only, so the locale changes
   nothing.  POSIX wants strerror_l thread-safe: the buffer is per
   thread where there is TLS.  */
char *strerror_l(int errnum, __locale_t loc)
{
#ifdef __UCLIBC_HAS_TLS__
	static __thread char buf[_STRERROR_BUFSIZE];
#else
	static char buf[_STRERROR_BUFSIZE];
#endif

	(void) loc;
	__xpg_strerror_r(errnum, buf, sizeof(buf));
	return buf;
}
