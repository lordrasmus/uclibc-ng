/* Dummy crti file.

   Empty like crtn.o: this port runs constructors and destructors from
   .init_array/.fini_array only, and crt1.S passes no _init/_fini to
   __uClibc_main.  An _init label here without a return in crtn.o
   gave every shared object linked with these files a DT_INIT that
   ran off the end of .init.  */
