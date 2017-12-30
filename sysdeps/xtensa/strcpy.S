/*
   ANSI C standard library function strcpy
   Xtensa Processor version.

   Copyright (C) 2001-2013 Free Software Foundation, Inc.
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


#include <sysdep.h>

#define XCHAL_HAVE_LOOPS 1

#if defined(__XTENSA_EL__)
#define MASK0 0x000000ff
#define MASK1 0x0000ff00
#define MASK2 0x00ff0000
#define MASK3 0xff000000
#elif defined(__XTENSA_EB__)
#define MASK0 0xff000000
#define MASK1 0x00ff0000
#define MASK2 0x0000ff00
#define MASK3 0x000000ff
#else
#error "Unsupported Xtensa endianness"
#endif

/*
 * char *strcpy(char *dst, const char *src)
 */
.text

# Register use
#   a0/ return address
#   a1/ stack pointer
#   a2/ return value
#   a3/ src
#   a4/ mask0
#   a5/ mask1
#   a6/ mask2
#   a7/ mask3
#   a8/ tmp
#   a9/ tmp
#   a10/ dst

ENTRY(strcpy)
	entry	sp, 16		# minimal stack frame
	# a2/ dst, a3/ src
	mov	a10, a2		# leave dst in return value register
	movi	a4, MASK0	# mask for byte 0
	movi	a5, MASK1	# mask for byte 1
	movi	a6, MASK2	# mask for byte 2
	movi	a7, MASK3	# mask for byte 3
	bbsi.l	a3, 0, .Lsrc1mod2 # if only  8-bit aligned
	bbsi.l	a3, 1, .Lsrc2mod4 # if only 16-bit aligned
.Lsrcaligned:	# return here when src is word-aligned
	movi	a8, 3
	bnone	a10, a8, .Laligned
	j	.Ldstunaligned

.Lsrc1mod2:	# src address is odd
	l8ui	a8, a3, 0	# get byte 0
	addi	a3, a3, 1	# advance src pointer
	s8i	a8, a10, 0	# store byte 0
	beqz	a8, .Lret	# if byte 0 is zero
	addi	a10, a10, 1	# advance dst pointer
	bbci.l	a3, 1, .Lsrcaligned # if src is now word-aligned

.Lsrc2mod4:	# src address is 2 mod 4
	l8ui	a8, a3, 0	# get byte 0
	/* 1-cycle interlock */
	s8i	a8, a10, 0	# store byte 0
	beqz	a8, .Lret	# if byte 0 is zero
	l8ui	a8, a3, 1	# get byte 0
	addi	a3, a3, 2	# advance src pointer
	s8i	a8, a10, 1	# store byte 0
	addi	a10, a10, 2	# advance dst pointer
	bnez	a8, .Lsrcaligned
.Lret:
	retw

/*
 * dst is word-aligned, src is word-aligned
 */
	.align	4		# (2 mod 4) alignment for loop instruction
.Laligned:
#if XCHAL_HAVE_LOOPS
	_movi.n	a8, 0		# set up for the maximum loop count
	loop	a8, .Lz3	# loop forever (almost anyway)
	l32i	a8, a3, 0	# get word from src
	addi	a3, a3, 4	# advance src pointer
	bnone	a8, a4, .Lz0	# if byte 0 is zero
	bnone	a8, a5, .Lz1	# if byte 1 is zero
	bnone	a8, a6, .Lz2	# if byte 2 is zero
	s32i	a8, a10, 0	# store word to dst
	bnone	a8, a7, .Lz3	# if byte 3 is zero
	addi	a10, a10, 4	# advance dst pointer

#else /* !XCHAL_HAVE_LOOPS */

	j	.Lfirstword
.Lnextword:
	addi	a10, a10, 4	# advance dst pointer
.Lfirstword:
	l32i	a8, a3, 0	# get word from src
	addi	a3, a3, 4	# advance src pointer
	bnone	a8, a4, .Lz0	# if byte 0 is zero
	bnone	a8, a5, .Lz1	# if byte 1 is zero
	bnone	a8, a6, .Lz2	# if byte 2 is zero
	s32i	a8, a10, 0	# store word to dst
	bany	a8, a7, .Lnextword	# if byte 3 is zero
#endif /* !XCHAL_HAVE_LOOPS */

.Lz3:	# byte 3 is zero
	retw
.Lz0:	# byte 0 is zero
#ifdef __XTENSA_EB__
	movi	a8, 0
#endif /* __XTENSA_EB__ */
	s8i	a8, a10, 0
	retw
.Lz1:	# byte 1 is zero
#ifdef __XTENSA_EB__
        extui   a8, a8, 16, 16
#endif /* __XTENSA_EB__ */
	s16i	a8, a10, 0
	retw
.Lz2:	# byte 2 is zero
#ifdef __XTENSA_EB__
        extui   a8, a8, 16, 16
#endif /* __XTENSA_EB__ */
	s16i	a8, a10, 0
	movi	a8, 0
	s8i	a8, a10, 2
	retw

	.align	4		# (2 mod 4) alignment for loop instruction
.Ldstunaligned:
#if 1
/*
 * for now just use byte copy loop
 */
#if XCHAL_HAVE_LOOPS
	_movi.n	a8, 0		# set up for the maximum loop count
	loop	a8, .Lunalignedend # loop forever (almost anyway)
#endif /* XCHAL_HAVE_LOOPS */
.Lnextbyte:
	l8ui	a8, a3, 0
	addi	a3, a3, 1
	s8i	a8, a10, 0
	addi	a10, a10, 1
#if XCHAL_HAVE_LOOPS
	beqz	a8, .Lunalignedend
#else /* !XCHAL_HAVE_LOOPS */
	bnez	a8, .Lnextbyte
#endif /* !XCHAL_HAVE_LOOPS */
.Lunalignedend:
	retw
#else /* 0 */
/*
 * this code is not functional yet
 */
	l32i	a9, a2, 0	# load word from dst
#ifdef __XTENSA_EB__
	ssa8b	a9		# rotate by dst alignment so that
	src	a9, a9, a9	# shift in loop will put back in place
	ssa8l	a9		# shift left by byte*8
#else /* !__XTENSA_EB__ */
	ssa8l	a9		# rotate by dst alignment so that
	src	a9, a9, a9	# shift in loop will put back in place
	ssa8b	a9		# shift left by 32-byte*8
#endif /* !__XTENSA_EB__ */
/*
 * dst is word-aligned, src is unaligned
 */
.Ldstunalignedloop:
	l32i	a8, a3, 0	# get word from src
	# 1-cycle interlock
	bnone	a8, a4, .Lu0	# if byte 0 is zero
	bnone	a8, a5, .Lu1	# if byte 1 is zero
	bnone	a8, a6, .Lu2	# if byte 2 is zero
	src	a9, a8, a9	# combine last word and this word
	s32i	a9, a10, 0	# store word to dst
	bnone	a8, a7, .Lu3	# if byte 3 is nonzero, iterate
	l32i	a9, a3, 4	# get word from src
	addi	a3, a3, 8	# advance src pointer
	bnone	a9, a4, .Lu4	# if byte 0 is zero
	bnone	a9, a5, .Lu5	# if byte 1 is zero
	bnone	a9, a6, .Lu6	# if byte 2 is zero
	src	a8, a9, a8	# combine last word and this word
	s32i	a8, a10, 4	# store word to dst
	addi	a10, a10, 8	# advance dst pointer
	bany	a8, a7, .Ldstunalignedloop # if byte 3 is nonzero, iterate
.Lu7:	# byte 7 is zero
	retw
.Lu0:	# byte 0 is zero
#ifdef __XTENSA_EB__
	movi	a8, 0
#endif /* __XTENSA_EB__ */
	s8i	a8, a10, 0
	retw
.Lu1:	# byte 1 is zero
#ifdef __XTENSA_EB__
        extui   a8, a8, 16, 16
#endif /* __XTENSA_EB__ */
	s16i	a8, a10, 0
	retw
.Lu2:	# byte 2 is zero
	s16i	a8, a10, 0
	movi	a8, 0
	s8i	a8, a10, 2
	retw
#endif /* 0 */
END(strcpy)
libc_hidden_builtin_def (strcpy)
