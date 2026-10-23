/* The floating-point environment of soft-float code, kept in software.

   libgcc's soft-fp asks for the rounding mode through __sfp_rounding_mode
   and hands over the exception flags of every operation through
   __sfp_raise_exceptions, both in the encoding of <fenv.h>.  The state has
   thread storage duration, as C11 7.6 requires.

   Licensed under the LGPL v2.1 or later, see the file COPYING.LIB
   in this tarball.  */

#ifndef _SFP_FENV_H
#define _SFP_FENV_H 1

#include <fenv.h>
#include <string.h>

struct __sfp_fenv
{
  unsigned int excepts;
  unsigned int round;
};

#ifdef __m68k__
/* m68k's lb1sf68.S keeps the environment itself, in _fpCCR: the sticky
   exception bits and the rounding mode, in the encoding of bits/fenv.h.  It
   is a single global, not one per thread.  */
extern struct
{
  unsigned short __exception_bits;
  unsigned short __trap_enable_bits;
  unsigned short __sticky_bits;
  unsigned short __rounding_mode;
} _fpCCR;
# define __SFP_EXCEPTS	_fpCCR.__sticky_bits
# define __SFP_ROUND	_fpCCR.__rounding_mode
#else
/* libgcc reads it on every soft-float operation, so no __tls_get_addr.  */
extern __thread struct __sfp_fenv __sfp_fenv
  attribute_hidden attribute_tls_model_ie;
# define __SFP_EXCEPTS	__sfp_fenv.excepts
# define __SFP_ROUND	__sfp_fenv.round
#endif

/* Whether ROUND is a mode the header offers.  */
static inline int
__sfp_valid_round (int round)
{
  return round == FE_TONEAREST
#ifdef FE_TOWARDZERO
	 || round == FE_TOWARDZERO
#endif
#ifdef FE_DOWNWARD
	 || round == FE_DOWNWARD
#endif
#ifdef FE_UPWARD
	 || round == FE_UPWARD
#endif
	 ;
}

/* fenv_t holds the flags in the low bits and the mode above them, which on
   riscv is the layout of fcsr.  */
#define __SFP_ROUND_SHIFT \
  (FE_ALL_EXCEPT ? 32 - __builtin_clz (FE_ALL_EXCEPT) : 0)

static inline void
__sfp_store_env (fenv_t *envp)
{
  unsigned int v = __SFP_EXCEPTS | (__SFP_ROUND << __SFP_ROUND_SHIFT);

  _Static_assert (sizeof (fenv_t) >= sizeof (v), "fenv_t too small");
  memcpy (envp, &v, sizeof (v));
}

static inline void
__sfp_load_env (const fenv_t *envp)
{
  unsigned int v;

  if (envp == FE_DFL_ENV)
    {
      __SFP_EXCEPTS = 0;
      __SFP_ROUND = FE_TONEAREST;
      return;
    }
  memcpy (&v, envp, sizeof (v));
  __SFP_EXCEPTS = v & FE_ALL_EXCEPT;
  __SFP_ROUND = v >> __SFP_ROUND_SHIFT;
}

#endif /* sfp-fenv.h */
