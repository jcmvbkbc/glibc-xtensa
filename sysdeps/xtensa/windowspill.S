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
        bbci.l  a0, 31, 4f		# branch if called with call4
        bbsi.l  a0, 30, 12f		# branch if called with call12

	# called with call8, touch register NUM_REGS-12 (4/20/52)

#if XCHAL_NUM_AREGS > 16
	call12	1f
	retw

	.align	4
1:	_entry	a1, 48			# touch NUM_REGS-24 (x/8/40)

#if XCHAL_NUM_AREGS == 32
	mov	a8, a0
	retw
#else
	_entry	a1, 48			# touch NUM_REGS-36 (x/x/28)
	mov	a12, a0
	_entry	a1, 48			# touch NUM_REGS-48 (x/x/16)
	mov	a12, a0
	_entry	a1, 16			# touch NUM_REGS-60 (x/x/4)
#endif
#endif
	mov	a4, a0
	retw

	# called with call4, touch register NUM_REGS-8 (8/24/56)
4:
#if XCHAL_NUM_AREGS == 16
	mov	a8, a0
#else
	call12	1f
	retw

	.align	4
1:	_entry	a1, 48			# touch NUM_REGS-20 (x/12/44)
	mov	a12, a0
#if XCHAL_NUM_AREGS > 32
	_entry	a1, 48			# touch NUM_REGS-32 (x/x/32)
	mov	a12, a0
	_entry	a1, 48			# touch NUM_REGS-44 (x/x/20)
	mov	a12, a0
	_entry	a1, 48			# touch NUM_REGS-56 (x/x/8)
	mov	a8, a0
#endif
#endif
	retw

	# called with call12, touch register NUM_REGS-16 (x/16/48)
12:
#if XCHAL_NUM_AREGS > 16
	call12	1f
	retw

	.align	4
1:	_entry	a1, 48			# touch NUM_REGS-28 (x/4/36)
#if XCHAL_NUM_AREGS == 32
	mov	a4, a0
#else
	mov	a12, a0
	_entry	a1, 48			# touch NUM_REGS-40 (x/x/24)
	mov	a12, a0
	_entry	a1, 48			# touch NUM_REGS-52 (x/x/12)
	mov	a12, a0
#endif
#endif
	retw
