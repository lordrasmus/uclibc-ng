/*
 * call_once() for uClibc (ISO C11 <threads.h>, C23 also <stdlib.h>)
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <stdlib.h>

#ifdef __UCLIBC_HAS_THREADS__
# include <pthread.h>

/* _Static_assert wants gcc 4.6 or C11, and avr32 is still built with
   gcc 4.4; the same two-way form as __ipc_assert in sysvipc/ipc.h.  */
# if ((defined __GNUC__							\
       && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)))	\
      || (defined __STDC_VERSION__ && __STDC_VERSION__ >= 201112L))
#  define __once_assert(expr, msg)	_Static_assert ((expr), msg)
# else
#  define __once_assert(expr, msg)					\
  extern int (*__once_assert_function (void))				\
    [!!sizeof (struct { int __error_if_negative: (expr) ? 2 : -1; })]
# endif

__once_assert (sizeof (once_flag) == sizeof (pthread_once_t),
	       "once_flag does not match pthread_once_t");
__once_assert (__alignof__ (once_flag) >= 4,
	       "once_flag is a futex word and must be 4-byte aligned");

void call_once(once_flag *flag, void (*func)(void))
{
	pthread_once((pthread_once_t *) flag, func);
}
#else
/* Without threads there is no one to race with.  */
void call_once(once_flag *flag, void (*func)(void))
{
	if (flag->__data == 0) {
		flag->__data = 1;
		func();
	}
}
#endif
