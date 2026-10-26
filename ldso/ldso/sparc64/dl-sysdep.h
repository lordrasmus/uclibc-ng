/*
 * sparc64 has no dynamic loader in uClibc-ng; static programs with NPTL
 * still include the ldso headers for the TLS setup.
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

/* Define this if the system uses RELOCA.  */
#define ELF_USES_RELOCA

#include <elf.h>
#include <link.h>

/* Here we define the magic numbers that this dynamic loader should accept */
#define MAGIC1 EM_SPARCV9
#undef  MAGIC2

/* Used for error messages */
#define ELF_TARGET "sparc64"

#define ELF_MACHINE_JMP_SLOT R_SPARC_JMP_SLOT

#define elf_machine_type_class(type)                           \
  ((ELF_RTYPE_CLASS_PLT * ((type) == ELF_MACHINE_JMP_SLOT      \
     || (type) == R_SPARC_TLS_DTPMOD64                         \
     || (type) == R_SPARC_TLS_DTPOFF64                         \
     || (type) == R_SPARC_TLS_TPOFF64))                        \
   | (ELF_RTYPE_CLASS_COPY * ((type) == R_SPARC_COPY)))
