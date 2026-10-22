/* POSIX asynchronous I/O: submit a list of requests.
   Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.

   Adapted from musl libc (src/aio/lio_listio.c), MIT licensed,
   Copyright (c) 2005-2020 Rich Felker, et al.  */

#include <aio.h>
#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>

struct lio_state {
	struct sigevent *sev;
	int cnt;
	struct aiocb *cbs[];
};

static int lio_wait(struct lio_state *st)
{
	int i, err, got_err = 0;
	int cnt = st->cnt;
	struct aiocb **cbs = st->cbs;

	for (;;) {
		for (i = 0; i < cnt; i++) {
			if (!cbs[i])
				continue;
			err = aio_error(cbs[i]);
			if (err == EINPROGRESS)
				break;
			if (err)
				got_err = 1;
			cbs[i] = 0;
		}
		if (i == cnt) {
			if (got_err) {
				__set_errno(EIO);
				return -1;
			}
			return 0;
		}
		if (aio_suspend((void *)cbs, cnt, 0))
			return -1;
	}
}

static void notify_signal(struct sigevent *sev)
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

static void *wait_thread(void *p)
{
	struct lio_state *st = p;
	struct sigevent *sev = st->sev;

	lio_wait(st);
	free(st);
	switch (sev->sigev_notify) {
	case SIGEV_SIGNAL:
		notify_signal(sev);
		break;
	case SIGEV_THREAD:
		sev->sigev_notify_function(sev->sigev_value);
		break;
	}
	return 0;
}

int lio_listio(int mode, struct aiocb *__restrict const cbs[__restrict],
	       int cnt, struct sigevent *__restrict sev)
{
	int i, ret;
	struct lio_state *st = 0;

	if (cnt < 0) {
		__set_errno(EINVAL);
		return -1;
	}

	if (mode == LIO_WAIT || (sev && sev->sigev_notify != SIGEV_NONE)) {
		if (!(st = malloc(sizeof *st + cnt * sizeof *cbs))) {
			__set_errno(EAGAIN);
			return -1;
		}
		st->cnt = cnt;
		st->sev = sev;
		memcpy(st->cbs, (void *)cbs, cnt * sizeof *cbs);
	}

	for (i = 0; i < cnt; i++) {
		if (!cbs[i])
			continue;
		switch (cbs[i]->aio_lio_opcode) {
		case LIO_READ:
			ret = aio_read(cbs[i]);
			break;
		case LIO_WRITE:
			ret = aio_write(cbs[i]);
			break;
		default:
			continue;
		}
		if (ret) {
			free(st);
			__set_errno(EAGAIN);
			return -1;
		}
	}

	if (mode == LIO_WAIT) {
		ret = lio_wait(st);
		free(st);
		return ret;
	}

	if (st) {
		pthread_attr_t a;
		sigset_t set, set_old;
		pthread_t td;

		if (sev->sigev_notify == SIGEV_THREAD) {
			if (sev->sigev_notify_attributes)
				a = *(pthread_attr_t *)sev->sigev_notify_attributes;
			else
				pthread_attr_init(&a);
		} else {
			pthread_attr_init(&a);
			pthread_attr_setstacksize(&a, PTHREAD_STACK_MIN);
			pthread_attr_setguardsize(&a, 0);
		}
		pthread_attr_setdetachstate(&a, PTHREAD_CREATE_DETACHED);
		sigfillset(&set);
		pthread_sigmask(SIG_BLOCK, &set, &set_old);
		ret = pthread_create(&td, &a, wait_thread, st);
		pthread_sigmask(SIG_SETMASK, &set_old, 0);
		if (ret) {
			free(st);
			__set_errno(EAGAIN);
			return -1;
		}
	}

	return 0;
}
