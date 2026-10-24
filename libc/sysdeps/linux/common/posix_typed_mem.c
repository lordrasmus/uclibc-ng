/*
 * posix_typed_mem_open, posix_typed_mem_get_info and posix_mem_offset,
 * POSIX.1-2008 (TYM).
 *
 * Linux has no typed memory objects, so no name names one, no
 * descriptor refers to one and no mapping comes from one.
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>

int posix_typed_mem_open(const char *name, int oflag, int tflag)
{
	(void) name;
	(void) oflag;
	/* At most one of the three flags.  */
	__set_errno(tflag & (tflag - 1) ? EINVAL : ENOENT);
	return -1;
}

int posix_typed_mem_get_info(int fildes, struct posix_typed_mem_info *info)
{
	(void) info;
	return fcntl(fildes, F_GETFD) < 0 ? EBADF : ENODEV;
}

int posix_mem_offset(const void *__restrict addr, size_t len,
		     off_t *__restrict off, size_t *__restrict contig_len,
		     int *__restrict fildes)
{
	(void) addr;
	(void) len;
	(void) off;
	(void) contig_len;
	(void) fildes;
	return EACCES;
}
