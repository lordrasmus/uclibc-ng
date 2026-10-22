/* POSIX asynchronous I/O, threads-based.
   Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.

   Adapted from musl libc (src/aio/aio.c), MIT licensed,
   Copyright (c) 2005-2020 Rich Felker, et al.

   Each file descriptor with outstanding operations has a queue,
   reference-counted and freed by the last worker thread.  Queues are
   found through a four-level table indexed by the descriptor; adding or
   removing a queue takes the table's write lock, locking an existing one
   its read lock.  Every operation runs in a thread of its own, with all
   signals blocked; completion is signalled with atomics and futexes so
   that waiters need no locks.  */

#include <aio.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <atomic.h>
#include "aio_misc.h"

/* Operations besides LIO_READ and LIO_WRITE; O_SYNC and O_DSYNC are the
   same value in some fcntl.h, so they cannot serve as case labels.  */
#define AIO_OP_FSYNC	(LIO_NOP + 1)
#define AIO_OP_FDSYNC	(LIO_NOP + 2)

struct aio_thread {
	pthread_t td;
	struct aiocb *cb;
	struct aio_thread *next, *prev;
	struct aio_queue *q;
	volatile int running __attribute__ ((__aligned__ (4)));
	int err, op;
	ssize_t ret;
};

struct aio_queue {
	int fd, seekable, append, ref, init;
	pthread_mutex_t lock;
	pthread_cond_t cond;
	struct aio_thread *head;
};

struct aio_args {
	struct aiocb *cb;
	struct aio_queue *q;
	int op;
	sem_t sem;
};

static pthread_rwlock_t maplock = PTHREAD_RWLOCK_INITIALIZER;
static struct aio_queue *****map;
volatile int __aio_fut __attribute__ ((__aligned__ (4)));

static void aio_prepare(void)
{
	pthread_rwlock_rdlock(&maplock);
}

static void aio_parent(void)
{
	pthread_rwlock_unlock(&maplock);
}

/* The queues and their worker threads belong to the parent; the child
   starts empty.  The lock is set up anew rather than unlocked, since the
   thread in the parent holding it does not exist here.  */
static void aio_child(void)
{
	int a, b, c, d;

	if (map) for (a = 0; a < (-1U/2+1)>>24; a++)
		if (map[a]) for (b = 0; b < 256; b++)
			if (map[a][b]) for (c = 0; c < 256; c++)
				if (map[a][b][c]) for (d = 0; d < 256; d++)
					map[a][b][c][d] = 0;
	pthread_rwlock_init(&maplock, 0);
}

static struct aio_queue *aio_get_queue(int fd, int need)
{
	sigset_t allmask, origmask;
	int masked = 0;
	int a;
	unsigned char b, c, d;
	struct aio_queue *q = 0;

	if (fd < 0) {
		__set_errno(EBADF);
		return 0;
	}
	a = fd >> 24;
	b = fd >> 16;
	c = fd >> 8;
	d = fd;
	pthread_rwlock_rdlock(&maplock);
	if ((!map || !map[a] || !map[a][b] || !map[a][b][c]
	     || !(q = map[a][b][c][d])) && need) {
		pthread_rwlock_unlock(&maplock);
		if (fcntl(fd, F_GETFD) < 0)
			return 0;
		sigfillset(&allmask);
		masked = 1;
		pthread_sigmask(SIG_BLOCK, &allmask, &origmask);
		pthread_rwlock_wrlock(&maplock);
		if (!map) {
			map = calloc(sizeof *map, (-1U/2+1)>>24);
			if (!map)
				goto out;
			pthread_atfork(aio_prepare, aio_parent, aio_child);
		}
		if (!map[a]) map[a] = calloc(sizeof **map, 256);
		if (!map[a]) goto out;
		if (!map[a][b]) map[a][b] = calloc(sizeof ***map, 256);
		if (!map[a][b]) goto out;
		if (!map[a][b][c]) map[a][b][c] = calloc(sizeof ****map, 256);
		if (!map[a][b][c]) goto out;
		if (!(q = map[a][b][c][d])) {
			map[a][b][c][d] = q = calloc(sizeof *****map, 1);
			if (q) {
				q->fd = fd;
				pthread_mutex_init(&q->lock, 0);
				pthread_cond_init(&q->cond, 0);
			}
		}
	}
	if (q)
		pthread_mutex_lock(&q->lock);
out:
	pthread_rwlock_unlock(&maplock);
	if (masked)
		pthread_sigmask(SIG_SETMASK, &origmask, 0);
	return q;
}

static void aio_unref_queue(struct aio_queue *q)
{
	if (q->ref > 1) {
		q->ref--;
		pthread_mutex_unlock(&q->lock);
		return;
	}

	/* Possibly the last reference, but freeing needs the map lock, and
	   taking that means dropping the queue lock first, during which a
	   new reference may come.  */
	pthread_mutex_unlock(&q->lock);
	pthread_rwlock_wrlock(&maplock);
	pthread_mutex_lock(&q->lock);
	if (q->ref == 1) {
		int fd = q->fd;
		map[fd >> 24][(unsigned char)(fd >> 16)][(unsigned char)(fd >> 8)]
			[(unsigned char)fd] = 0;
		pthread_rwlock_unlock(&maplock);
		pthread_mutex_unlock(&q->lock);
		free(q);
	} else {
		q->ref--;
		pthread_rwlock_unlock(&maplock);
		pthread_mutex_unlock(&q->lock);
	}
}

static void aio_notify_signal(const struct sigevent *sev)
{
	siginfo_t si = {
		.si_signo = sev->sigev_signo,
		.si_code = SI_ASYNCIO,
	};

	si.si_value = sev->sigev_value;
	si.si_pid = getpid();
	si.si_uid = getuid();
	INTERNAL_SYSCALL_DECL(err);
	INTERNAL_SYSCALL(rt_sigqueueinfo, err, 3, si.si_pid, si.si_signo, &si);
}

static void cleanup(void *ctx)
{
	struct aio_thread *at = ctx;
	struct aio_queue *q = at->q;
	struct aiocb *cb = at->cb;
	struct sigevent sev = cb->aio_sigevent;

	/* Waiters to wake: aio_cancel, aio_suspend on this request alone,
	   aio_suspend on a list -- all through atomics and futexes -- and
	   workers waiting for sequenced operations, through the cond var.  */
	cb->__ret = at->ret;
	if (atomic_exchange_acq(&at->running, 0) < 0)
		__aio_wake(&at->running);
	if (atomic_exchange_acq(&cb->__err, at->err) != EINPROGRESS)
		__aio_wake(&cb->__err);
	if (atomic_exchange_acq(&__aio_fut, 0))
		__aio_wake(&__aio_fut);

	pthread_mutex_lock(&q->lock);

	if (at->next) at->next->prev = at->prev;
	if (at->prev) at->prev->next = at->next;
	else q->head = at->next;

	pthread_cond_broadcast(&q->cond);

	aio_unref_queue(q);

	if (sev.sigev_notify == SIGEV_SIGNAL)
		aio_notify_signal(&sev);
	if (sev.sigev_notify == SIGEV_THREAD) {
		/* aio_cancel may have cancelled this thread; the callback
		   still has to run, and to run to completion.  */
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0);
		sev.sigev_notify_function(sev.sigev_value);
	}
}

static void *io_thread_func(void *ctx)
{
	struct aio_thread at, *p;
	struct aio_args *args = ctx;
	struct aiocb *cb = args->cb;
	int fd = cb->aio_fildes;
	int op = args->op;
	void *buf = (void *)cb->aio_buf;
	size_t len = cb->aio_nbytes;
	__off64_t off = cb->aio_offset;
	struct aio_queue *q = args->q;
	ssize_t ret = -1;

	pthread_mutex_lock(&q->lock);
	sem_post(&args->sem);

	at.op = op;
	at.running = 1;
	at.ret = -1;
	at.err = ECANCELED;
	at.q = q;
	at.td = pthread_self();
	at.cb = cb;
	at.prev = 0;
	if ((at.next = q->head))
		at.next->prev = &at;
	q->head = &at;

	if (!q->init) {
		int seekable = lseek64(fd, 0, SEEK_CUR) >= 0;
		q->seekable = seekable;
		q->append = !seekable || (fcntl(fd, F_GETFL) & O_APPEND);
		q->init = 1;
	}

	pthread_cleanup_push(cleanup, &at);

	/* Everything but a positioned read or write waits for the writes
	   queued before it.  */
	if (op != LIO_READ && (op != LIO_WRITE || q->append)) {
		for (;;) {
			for (p = at.next; p && p->op != LIO_WRITE; p = p->next)
				;
			if (!p)
				break;
			pthread_cond_wait(&q->cond, &q->lock);
		}
	}

	pthread_mutex_unlock(&q->lock);

	switch (op) {
	case LIO_WRITE:
		ret = q->append ? write(fd, buf, len) : pwrite64(fd, buf, len, off);
		break;
	case LIO_READ:
		ret = !q->seekable ? read(fd, buf, len) : pread64(fd, buf, len, off);
		break;
	case AIO_OP_FSYNC:
		ret = fsync(fd);
		break;
	case AIO_OP_FDSYNC:
		ret = fdatasync(fd);
		break;
	}
	at.ret = ret;
	at.err = ret < 0 ? errno : 0;

	pthread_cleanup_pop(1);

	return 0;
}

static int submit(struct aiocb *cb, int op)
{
	int ret = 0;
	pthread_attr_t a;
	sigset_t allmask, origmask;
	pthread_t td;
	struct aio_queue *q = aio_get_queue(cb->aio_fildes, 1);
	struct aio_args args = { .cb = cb, .op = op, .q = q };

	sem_init(&args.sem, 0, 0);

	if (!q) {
		if (errno != EBADF)
			__set_errno(EAGAIN);
		cb->__ret = -1;
		cb->__err = errno;
		return -1;
	}
	q->ref++;
	pthread_mutex_unlock(&q->lock);

	if (cb->aio_sigevent.sigev_notify == SIGEV_THREAD) {
		if (cb->aio_sigevent.sigev_notify_attributes)
			a = *(pthread_attr_t *)cb->aio_sigevent.sigev_notify_attributes;
		else
			pthread_attr_init(&a);
	} else {
		pthread_attr_init(&a);
		pthread_attr_setstacksize(&a, PTHREAD_STACK_MIN);
		pthread_attr_setguardsize(&a, 0);
	}
	pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED);
	sigfillset(&allmask);
	pthread_sigmask(SIG_BLOCK, &allmask, &origmask);
	cb->__err = EINPROGRESS;
	if (pthread_create(&td, &a, io_thread_func, &args)) {
		pthread_mutex_lock(&q->lock);
		aio_unref_queue(q);
		cb->__err = EAGAIN;
		__set_errno(EAGAIN);
		cb->__ret = ret = -1;
	}
	pthread_sigmask(SIG_SETMASK, &origmask, 0);

	if (!ret)
		while (sem_wait(&args.sem))
			;

	return ret;
}

int aio_read(struct aiocb *cb)
{
	return submit(cb, LIO_READ);
}

int aio_write(struct aiocb *cb)
{
	return submit(cb, LIO_WRITE);
}

int aio_fsync(int op, struct aiocb *cb)
{
	if (op != O_SYNC && op != O_DSYNC) {
		__set_errno(EINVAL);
		return -1;
	}
	return submit(cb, op == O_SYNC ? AIO_OP_FSYNC : AIO_OP_FDSYNC);
}

ssize_t aio_return(struct aiocb *cb)
{
	return cb->__ret;
}

int aio_error(const struct aiocb *cb)
{
	atomic_full_barrier();
	/* bit 31 marks a waiter in aio_suspend */
	return cb->__err & 0x7fffffff;
}

int aio_cancel(int fd, struct aiocb *cb)
{
	sigset_t allmask, origmask;
	int ret = AIO_ALLDONE;
	struct aio_thread *p;
	struct aio_queue *q;

	if (cb && fd != cb->aio_fildes) {
		__set_errno(EINVAL);
		return -1;
	}

	sigfillset(&allmask);
	pthread_sigmask(SIG_BLOCK, &allmask, &origmask);

	__set_errno(ENOENT);
	if (!(q = aio_get_queue(fd, 0))) {
		if (errno == EBADF)
			ret = -1;
		goto done;
	}

	for (p = q->head; p; p = p->next) {
		if (cb && cb != p->cb)
			continue;
		/* running -> running with a waiter */
		if (atomic_compare_and_exchange_val_acq(&p->running, -1, 1)) {
			pthread_cancel(p->td);
			while (p->running)
				__aio_futex_wait(&p->running, -1, NULL);
			if (p->err == ECANCELED)
				ret = AIO_CANCELED;
		}
	}

	pthread_mutex_unlock(&q->lock);
done:
	pthread_sigmask(SIG_SETMASK, &origmask, 0);
	return ret;
}
