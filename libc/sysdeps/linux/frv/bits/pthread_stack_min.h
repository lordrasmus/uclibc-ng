/* Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.  */

/* Minimum size for a thread.  Two pages: FR-V Linux uses 16k pages, and
   the guard page alone takes one.  */
#define PTHREAD_STACK_MIN	32768
