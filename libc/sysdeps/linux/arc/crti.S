/* Dummy crti file.

   Empty, as glibc's sysdeps/generic/crti.S for its newer ports: gcc's
   crtbegin.o and crtend.o on arc run constructors and destructors from
   .init_array/.fini_array, and crt1.S references _init and _fini only
   weakly, so __uClibc_main gets NULL when nothing defines them.  */
