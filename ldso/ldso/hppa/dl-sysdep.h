/*
 * hppa has no dynamic loader in uClibc-ng; static programs with NPTL
 * still include the ldso headers for the TLS setup.
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

/* Define this if the system uses RELOCA.  */
#define ELF_USES_RELOCA

#include <elf.h>
#include <link.h>

/* Here we define the magic numbers that this dynamic loader should accept */
#define MAGIC1 EM_PARISC
#undef  MAGIC2

/* Used for error messages */
#define ELF_TARGET "hppa"

#define ELF_MACHINE_JMP_SLOT R_PARISC_IPLT

#define elf_machine_type_class(type)                           \
  ((ELF_RTYPE_CLASS_PLT * ((type) == ELF_MACHINE_JMP_SLOT))    \
   | (ELF_RTYPE_CLASS_COPY * ((type) == R_PARISC_COPY)))
