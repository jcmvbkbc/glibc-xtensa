/* Machine-dependent definitions for profiling support.  Nios II version.
   Copyright (C) 1996-2017 Free Software Foundation, Inc.
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

#include <sysdep.h>

#define _MCOUNT_DECL(frompc, selfpc) \
static void __attribute_used__ __mcount_internal (u_long frompc, u_long selfpc)

/* This macro/func MUST save r4, r5, r6, r7 and r8 because the compiler inserts
   blind calls to mcount(), ignoring the fact that mcount may clobber
   registers; therefore, mcount may NOT clobber registers.  */

#define MCOUNT						\
  asm (".globl _mcount\n\t"				\
       ".type _mcount,@function\n\t"			\
       "_mcount:\n\t"					\
       "entry sp, 16\n\t"				\
       "retw\n\t"					\
       ".size _mcount, . - _mcount\n\t"			\
       );
