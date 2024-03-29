/*  Copyright (C) 2004     Erik Andersen
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  The GNU C Library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with the GNU C Library; see the file COPYING.LIB.  If
 *  not, see <http://www.gnu.org/licenses/>.
 */

/* Supply an architecture specific value for PAGE_SIZE and friends.  */

#ifndef _UCLIBC_PAGE_H
#define _UCLIBC_PAGE_H

/* PAGE_SHIFT determines the page size -- in this case 4096 */
#define PAGE_SHIFT	12
#define PAGE_SIZE	(1UL << PAGE_SHIFT)
#define PAGE_MASK	(~(PAGE_SIZE-1))

/* Some architectures always use 12 as page shift for mmap2() eventhough the
 * real PAGE_SHIFT != 12.  Other architectures use the same value as
 * PAGE_SHIFT...
 */
#define MMAP2_PAGE_SHIFT PAGE_SHIFT

#endif /* _UCLIBC_PAGE_H */
