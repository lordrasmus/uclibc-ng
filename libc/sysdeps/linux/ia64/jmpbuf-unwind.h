/*
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 */

#include <setjmp.h>

/* Test if longjmp to JMPBUF would unwind the frame containing a local
   variable at ADDRESS.  */
#define _JMPBUF_UNWINDS(_jmpbuf, _address)		\
     ((void *)(_address) < (void *)(((long *)_jmpbuf)[0]))

#ifdef __UCLIBC_HAS_THREADS_NATIVE__
#include <stdint.h>
#include <unwind.h>

/* A frame with the jmp_buf's CFA is still unwound if its register stack
   lies above the saved ar.bsp (word 17).  */
#define _JMPBUF_CFA_UNWINDS_ADJ(_jmpbuf, _context, _adj) \
  ({ uintptr_t _cfa = (uintptr_t) _Unwind_GetCFA (_context) - (_adj);	\
     (_cfa < (uintptr_t)(((long *)(_jmpbuf))[0]) - (_adj)		\
      || (_cfa == (uintptr_t)(((long *)(_jmpbuf))[0]) - (_adj)		\
	  && (uintptr_t) _Unwind_GetBSP (_context) - (_adj)		\
	     >= (uintptr_t)(((long *)(_jmpbuf))[17]) - (_adj)));	\
  })

#define _JMPBUF_UNWINDS_ADJ(_jmpbuf, _address, _adj) \
  ((uintptr_t)(_address) - (_adj) < (uintptr_t)(((long *)_jmpbuf)[0]) - (_adj))
#endif
