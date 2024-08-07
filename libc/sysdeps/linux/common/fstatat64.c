/*
 * fstatat64() for uClibc
 *
 * Copyright (C) 2009 Analog Devices Inc.
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <_lfs_64.h>
#include <bits/wordsize.h>
#include <sys/syscall.h>
#include <linux/version.h>

#if defined __mips__
# include <sgidefs.h>
#endif

/* 64bit ports tend to favor newfstatat() */
#if __WORDSIZE == 64 && defined __NR_newfstatat
# define __NR_fstatat64 __NR_newfstatat
#endif
/* mips N32 ABI use newfstatat(), too */
#if defined __mips__ && _MIPS_SIM == _ABIN32
# define __NR_fstatat64 __NR_newfstatat
#endif

#if defined(__NR_fstatat64) && (!defined(__UCLIBC_USE_TIME64__)  || LINUX_VERSION_CODE <= KERNEL_VERSION(5,1,0))
# include <sys/stat.h>
# include "xstatconv.h"
int fstatat64(int fd, const char *file, struct stat64 *buf, int flag)
{
# ifdef __ARCH_HAS_DEPRECATED_SYSCALLS__
	int ret;
	struct kernel_stat64 kbuf;

	ret = INLINE_SYSCALL(fstatat64, 4, fd, file, &kbuf, flag);
	if (ret == 0)
		__xstat64_conv(&kbuf, buf);

	return ret;
# else
	return INLINE_SYSCALL(fstatat64, 4, fd, file, buf, flag);
# endif
}
libc_hidden_def(fstatat64)
#else

#if defined(__NR_statx) && defined(__UCLIBC_HAVE_STATX__)
# include <sys/stat.h>
# include <statx_cp.h>
# include <fcntl.h> // for AT_NO_AUTOMOUNT

int fstatat64(int fd, const char *file, struct stat64 *buf, int flag)
{
	struct statx tmp;

	int r = INLINE_SYSCALL(statx, 5, fd, file, AT_NO_AUTOMOUNT | flag,
			       STATX_BASIC_STATS, &tmp);

	if (r == 0)
		__cp_stat64_statx ((struct stat64 *)buf, &tmp);

	return r;
}
libc_hidden_def(fstatat64)
#endif

/* should add emulation with fstat64() and /proc/self/fd/ ... */
#endif
