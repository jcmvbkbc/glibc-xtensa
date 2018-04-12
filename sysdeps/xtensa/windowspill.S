/*

   Copyright (C) 2005-2013 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/*
Flush always 64 registers even if only 32 registers are available
this ensures that window spill works in both 32 and 64 register
cases.
*/
#undef XCHAL_NUM_AREGS
#define XCHAL_NUM_AREGS 64

.text
.align  4
.global __window_spill
.type   __window_spill,@function
__window_spill:
	entry	a1, 48
#if XCHAL_NUM_AREGS > 16
	call12	1f
	_j	2f
	retw
	.align	4
1:
	_entry	a1, 48
	addi	a12, a0, 3
#if XCHAL_NUM_AREGS > 32
	.rept	(XCHAL_NUM_AREGS - 32) / 12
	_entry	a1, 48
	mov	a12, a0
	.endr
#endif
	_entry	a1, 16
#if XCHAL_NUM_AREGS % 12 == 0
	mov	a8, a8
#elif XCHAL_NUM_AREGS % 12 == 4
	mov	a12, a12
#elif XCHAL_NUM_AREGS % 12 == 8
	mov	a4, a4
#endif
	retw
2:
#else
	mov	a12, a12
#endif
	retw

.size __window_spill, . - __window_spill
