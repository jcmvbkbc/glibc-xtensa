/* PLT trampolines.  XTENSA version.
   Copyright (C) 2005-2013 Free Software Foundation, Inc.
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

/* This code is used in dl-runtime.c to call the `fixup' function
   and then redirect to the address it returns.  The trampoline is reached
   via an indirect jump from a stub function.  At entry to the trampoline:

	a10 = link map
	a11 = offset of the reloc entry

   These are the first two arguments to `fixup'.  The third argument is the
   return address, which we have to fix up by setting the high bits from
   the value of the current PC.

   The return value from `fixup' is the target address, but we can't simply
   jump to it.  The stub function has already done an ENTRY with a minimal
   stack frame to get some free regs.  We can't execute an extra ENTRY
   without messing up the register windows, so we have to fix up the current
   stack frame size and then jump to the next instruction past the target
   function's ENTRY. */

#include <sysdep.h>

#define MIN_FRAME_SIZE -32

#ifdef __XTENSA_EB__
#define XTENSA_IMM12_FLD_OFFSET 8
#else /* __XTENSA_EL__ */
#define XTENSA_IMM12_FLD_OFFSET 12
#endif /* __XTENSA_EL__ */

.macro	trampoline tramp_name fixup_name
	.text
	.literal_position
	.align	4
	.global	\tramp_name 
	.type	\tramp_name, @function
\tramp_name :
	# fix up the high 2 bits of the return address
	mov	a14, a0		# save a0 temporarily
	_call0	0f		# use a call0 to get the PC
	.align	4
0:	extui	a13, a0, 30, 2
	slli	a13, a13, 30
	mov	a0, a14		# restore a0
	slli	a12, a0, 2
	srli	a12, a12, 2
	or	a12, a12, a13
	# call the fixup function in dl-runtime.c
	movi	a8,  \fixup_name 
	callx8	a8
	# extract the target's frame size from the ENTRY instruction
	l32i	a11, a10, 0
	extui	a11, a11,  XTENSA_IMM12_FLD_OFFSET , 12
	slli	a11, a11, 3
	addi	a11, a11,  MIN_FRAME_SIZE 
	sub	a11, sp, a11
	movsp	sp, a11
	# jump to the next instruction past the ENTRY
	addi	a10, a10, 3
	jx	a10
	.size	 \tramp_name , . -  \tramp_name
.endm

#ifndef PROF
	trampoline _dl_runtime_resolve _dl_fixup
	trampoline _dl_runtime_profile _dl_profile_fixup
#else
	trampoline _dl_runtime_resolve _dl_fixup
	trampoline _dl_runtime_profile _dl_profile_fixup
#endif
