/* Helpers for the ISO C23 functions in libm/c23.

   Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.  */

#ifndef _C23_PRIVATE_H
#define _C23_PRIVATE_H 1

#include <math.h>
#include <float.h>
#include <stdint.h>
#include <string.h>
#include "math_private.h"

/* The long double functions compute in double, like the rest of libm
   (see ldouble_wrappers.c); only the ones that look at the encoding
   know the long double formats.  */
#if defined __UCLIBC_HAS_LONG_DOUBLE_MATH__ && !defined __NO_LONG_DOUBLE_MATH
# define C23_LDBL 1
#else
# define C23_LDBL 0
#endif

static __inline uint64_t c23_asuint64(double x)
{
	uint64_t i;

	memcpy(&i, &x, sizeof i);
	return i;
}

static __inline double c23_asdouble(uint64_t i)
{
	double x;

	memcpy(&x, &i, sizeof x);
	return x;
}

static __inline uint32_t c23_asuint32(float x)
{
	uint32_t i;

	memcpy(&i, &x, sizeof i);
	return i;
}

static __inline float c23_asfloat(uint32_t i)
{
	float x;

	memcpy(&x, &i, sizeof x);
	return x;
}

/* hppa and mips before nan2008 mark a signaling NaN by setting the high
   significand bit, the reverse of IEEE 754-2008.  */
#if defined __hppa__ || (defined __mips__ && !defined __mips_nan2008)
# define C23_SNAN_HIGH_BIT 1
#else
# define C23_SNAN_HIGH_BIT 0
#endif

/* The long double encoding as sign and a magnitude that orders like
   the value: hi first, then lo.  Formats: like double (53), the 80-bit
   extended of x87 and m68k (64), binary128 (113).  */
#if C23_LDBL && LDBL_MANT_DIG != 53
# include <endian.h>
# if LDBL_MANT_DIG == 64 || LDBL_MANT_DIG == 113
#  define C23_LDBL_BITS 1
struct c23_ldbits {
	int sign;
	uint64_t hi, lo;
};

static __inline void c23_ldget(const long double *x, struct c23_ldbits *b)
{
	unsigned char buf[sizeof(long double)];
#  if LDBL_MANT_DIG == 64
	uint16_t se;
	uint64_t m;

	memcpy(buf, x, sizeof buf);
#   if __BYTE_ORDER == __LITTLE_ENDIAN		/* x87 */
	memcpy(&m, buf, 8);
	memcpy(&se, buf + 8, 2);
#   else					/* m68k: exponent, pad, mantissa */
	memcpy(&se, buf, 2);
	memcpy(&m, buf + 4, 8);
#   endif
	b->sign = se >> 15;
	b->hi = se & 0x7fff;
	b->lo = m;
#  else
	uint64_t w[2];

	memcpy(buf, x, sizeof buf);
	memcpy(w, buf, 16);
#   if __BYTE_ORDER == __LITTLE_ENDIAN
	b->hi = w[1];
	b->lo = w[0];
#   else
	b->hi = w[0];
	b->lo = w[1];
#   endif
	b->sign = b->hi >> 63;
	b->hi &= ~(1ULL << 63);
#  endif
}

static __inline void c23_ldput(long double *x, const struct c23_ldbits *b)
{
	unsigned char buf[sizeof(long double)];
#  if LDBL_MANT_DIG == 64
	uint16_t se = (b->sign << 15) | (uint16_t) b->hi;

	memcpy(buf, x, sizeof buf);
#   if __BYTE_ORDER == __LITTLE_ENDIAN
	memcpy(buf, &b->lo, 8);
	memcpy(buf + 8, &se, 2);
#   else
	memcpy(buf, &se, 2);
	memcpy(buf + 4, &b->lo, 8);
#   endif
#  else
	uint64_t w[2], hi = b->hi | ((uint64_t) b->sign << 63);

#   if __BYTE_ORDER == __LITTLE_ENDIAN
	w[1] = hi;
	w[0] = b->lo;
#   else
	w[0] = hi;
	w[1] = b->lo;
#   endif
	memcpy(buf, w, 16);
#  endif
	memcpy(x, buf, sizeof buf);
}

/* Is the magnitude a signaling NaN?  */
static __inline int c23_ldsnan(const struct c23_ldbits *b)
{
#  if LDBL_MANT_DIG == 64
	return b->hi == 0x7fff && !(b->lo & (1ULL << 62))
	       && (b->lo & ((1ULL << 62) - 1));
#  elif C23_SNAN_HIGH_BIT
	return (b->hi >> 47) == 0xffff;
#  else
	return (b->hi >> 48) == 0x7fff && !(b->hi & (1ULL << 47))
	       && ((b->hi & ((1ULL << 47) - 1)) | b->lo);
#  endif
}
# endif
#endif

/* Raise invalid and return a quiet NaN, for a domain error.  */
static __inline double c23_invalid(void)
{
	volatile double z = 0.0;

	return z / z;
}

/* Raise divide-by-zero and return an infinity with the sign of S.  */
static __inline double c23_pole(double s)
{
	volatile double z = 0.0;

	return s / z;
}

#endif
