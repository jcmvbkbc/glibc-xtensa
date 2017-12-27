/* Copyright (C) 2011-2013 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library.  If not, see
   <http://www.gnu.org/licenses/>.  */

#ifndef _FENV_H
# error "Never use <bits/fenv.h> directly; include <fenv.h> instead."
#endif

/* Define bits representing exceptions in the FPU status word.  */
enum
  {
    FE_INEXACT = 1 << 7,
#define FE_INEXACT FE_INEXACT
    FE_UNDERFLOW = 1 << 8,
#define FE_UNDERFLOW FE_UNDERFLOW
    FE_OVERFLOW = 1 << 9,
#define FE_OVERFLOW FE_OVERFLOW
    FE_DIVBYZERO = 1 << 10,
#define FE_DIVBYZERO FE_DIVBYZERO
    FE_INVALID = 1 << 11,
#define FE_INVALID FE_INVALID
  };

/* Shift amount to convert an exception to a mask bit.  */
#define FE_EXCEPT_SHIFT 7

#define FE_ALL_EXCEPT \
	(FE_INEXACT | FE_UNDERFLOW | FE_OVERFLOW | FE_DIVBYZERO | FE_INVALID)

enum
  {
    FE_TONEAREST = 0,
#define FE_TONEAREST FE_TONEAREST
    FE_TOWARDZERO = 1,
#define FE_TOWARDZERO FE_TOWARDZERO
    FE_UPWARD = 2,
#define FE_UPWARD FE_UPWARD
    FE_DOWNWARD = 4,
#define FE_DOWNWARD FE_DOWNWARD
  };

/* Type representing exception flags (if there were any).  */
typedef unsigned int fexcept_t;

/* Type representing floating-point environment.  */
typedef unsigned int fenv_t;

/* If the default argument is used we use this value.  */
#define FE_DFL_ENV	((const fenv_t *) -1l)

#ifdef __USE_GNU
/* Floating-point environment where non of the exception is masked.  */
# define FE_NOMASK_ENV	((const fenv_t *) -127)
#endif
