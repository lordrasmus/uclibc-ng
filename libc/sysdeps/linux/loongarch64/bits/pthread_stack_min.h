/* Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.  */

/* Minimum size for a thread.  LoongArch kernels default to 16k pages and
   allow 64k, so a guard page alone can take 16384.  Same value as glibc.  */
#define PTHREAD_STACK_MIN	131072
