/* POSIX asynchronous I/O.
   Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.  */

#ifndef _AIO_H
#define _AIO_H	1

#include <features.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>

__BEGIN_DECLS

struct aiocb
{
  int aio_fildes;
  int aio_lio_opcode;
  int aio_reqprio;
  volatile void *aio_buf;
  size_t aio_nbytes;
  struct sigevent aio_sigevent;
  __off64_t aio_offset;

  /* Private.  */
  volatile int __err;
  ssize_t __ret;
};

/* Return values of aio_cancel.  */
#define AIO_CANCELED	0
#define AIO_NOTCANCELED	1
#define AIO_ALLDONE	2

/* Operation codes for aio_lio_opcode.  */
#define LIO_READ	0
#define LIO_WRITE	1
#define LIO_NOP		2

/* Modes of lio_listio.  */
#define LIO_WAIT	0
#define LIO_NOWAIT	1

extern int aio_read (struct aiocb *__aiocbp);
extern int aio_write (struct aiocb *__aiocbp);
extern int aio_fsync (int __op, struct aiocb *__aiocbp);
extern int aio_error (const struct aiocb *__aiocbp) __THROW;
extern ssize_t aio_return (struct aiocb *__aiocbp) __THROW;
extern int aio_cancel (int __fildes, struct aiocb *__aiocbp);
extern int aio_suspend (const struct aiocb *const __list[], int __nent,
			const struct timespec *__restrict __timeout);
extern int lio_listio (int __mode, struct aiocb *__restrict const __list[__restrict],
		       int __nent, struct sigevent *__restrict __sig);

#ifdef __USE_LARGEFILE64
/* aio_offset is 64 bits wide already.  */
# define aiocb64	aiocb
# define aio_read64	aio_read
# define aio_write64	aio_write
# define aio_fsync64	aio_fsync
# define aio_error64	aio_error
# define aio_return64	aio_return
# define aio_cancel64	aio_cancel
# define aio_suspend64	aio_suspend
# define lio_listio64	lio_listio
#endif

__END_DECLS

#endif /* aio.h */
