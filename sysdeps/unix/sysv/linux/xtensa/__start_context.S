/* Copyright (C) 2018 Free Software Foundation, Inc.
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
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <sysdep.h>

/*
 * There's no entry instruction, makecontext sets up ucontext_t as if
 * getcontext was called above and is about to return here.
 * Registers on entry to this function:
 *   a2: func to call
 *   a3: ucp->uc_link, next context to activate if func returns
 *   a4: func argc
 *   a5..a7: func arguments 0..2
 *
 */
	.literal_position

ENTRY(__start_context)
	mov	a10, a5
	mov	a11, a6
	mov	a12, a7
	bltui	a4, 4, 1f

	/* load func arguments 3..5 from stack and free that space */
	l32i	a13, a1, 4
	l32i	a14, a1, 8
	l32i	a15, a1, 12
	addi	a5, a1, 16
	movsp	a1, a5
	/* func arguments 6..argc - 1 are now at the top of the stack */
1:
	callx8	a2
	beqz	a3, 1f
	mov	a6, a3
	movi	a4, JUMPTARGET (setcontext)
	callx4	a4
1:
	movi	a4, JUMPTARGET (_exit)
	movi	a6, 0
	callx4	a4
	ill
END(__start_context)
