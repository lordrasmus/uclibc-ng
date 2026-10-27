/* Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.  */

/* gcc implements the __atomic builtins with the LL/SC sequences of the
   P flag on CRIS v32 and of the F flag on v10 (gcc/config/cris/sync.md).
   They trap on unaligned words, so v10 takes them only with NPTL, whose
   lock types are aligned (__LOCK_ALIGNMENT).  */
#if defined __arch_v32 || defined __UCLIBC_HAS_THREADS_NATIVE__
# include <bits/atomic-builtins.h>
#else
# include_next <bits/atomic.h>
#endif
