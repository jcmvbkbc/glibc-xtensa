/*
   ANSI C standard library function strcmp
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
#define MASK4 0x40404040

/*
 * int strcmp(const char *s1, const char *s2)
 */
.text

/*
 * Register use
 *   a0/ return address
 *   a1/ stack pointer
 *   a2/ s1
 *   a3/ s2
 *   a4/ mask0
 *   a5/ mask1
 *   a6/ mask2
 *   a7/ mask3 or mask4
 *   a8/ tmp
 *   a9/ tmp
 *   a10/ tmp
 */

ENTRY(strcmp)
	entry	sp, 16		# minimal stack frame
	/* a2/ s1, a3/ s2 */


	movi	a10, 3		# mask
        or	a8, a2, a3
        bnone   a8, a10, .Laligned

	xor	a8, a2, a3	# compare low two bits of s1 and s2
	bany	a8, a10, .Lunaligned	# if s1 and s2 have different alignment

	/* s1/s2 are not word-aligned */
	l8ui	a8, a2, 0	# byte 0 from s1
	l8ui	a9, a3, 0	# byte 0 from s2
	addi	a2, a2, 1	# advance s1
	bne	a8, a9, .Lretdiff # if different, return difference
	beqz	a8, .Leq	# bytes equal, if zero, strings are equal
	addi	a3, a3, 1	# advance s2
	bnone	a2, a10, .Laligned # if s1/s2 now aligned
	l8ui	a8, a2, 0	# byte 1 from s1
	l8ui	a9, a3, 0	# byte 1 from s2
	addi	a2, a2, 1	# advance s1
	bne	a8, a9, .Lretdiff # if different, return difference
	beqz	a8, .Leq	# bytes equal, if zero, strings are equal
	addi	a3, a3, 1	# advance s2
	bnone	a2, a10, .Laligned # if s1/s2 now aligned
	l8ui	a8, a2, 0	# byte 2 from s1
	l8ui	a9, a3, 0	# byte 2 from s2
	addi	a2, a2, 1	# advance s1
	bne	a8, a9, .Lretdiff # if different, return difference
	beqz	a8, .Leq	# bytes equal, if zero, strings are equal
	addi	a3, a3, 1	# advance s2
	j	.Laligned

/*
 * s1 and s2 have different alignment
 *
 * If the zero-overhead loop option is available, use an (almost)
 * infinite zero-overhead loop with conditional exits so we only pay
 * for taken branches when exiting the loop
 *
 * Note: It is important for this unaligned case to come before the
 * code for aligned strings, because otherwise some of the branches
 * above cannot reach and have to be transformed to branches around
 * jumps.  The unaligned code is smaller and the branches can reach
 * over it.
 */

	.align	4		# (2 mod 4) alignment for loop instruction
.Lunaligned:
#if XCHAL_HAVE_LOOPS
	movi	a11, 0		# set up for the maximum loop count
	loop	a11, .Lretdiff	# loop forever (almost anyway)
#endif /* XCHAL_HAVE_LOOPS */
.Lnextbyte:
	l8ui	a8, a2, 0
	l8ui	a9, a3, 0
	addi	a2, a2, 1
	bne	a8, a9, .Lretdiff
	addi	a3, a3, 1
#if XCHAL_HAVE_LOOPS
	beqz	a8, .Lretdiff
#else /* !XCHAL_HAVE_LOOPS */
	bnez	a8, .Lnextbyte
#endif /* !XCHAL_HAVE_LOOPS */
.Lretdiff:
	sub	a2, a8, a9
	retw

/*
 * s1 is word-aligned, s2 is word-aligned
 *
 * If the zero-overhead loop option is available, use an (almost)
 * infinite zero-overhead loop with conditional exits so we only pay
 * for taken branches when exiting the loop
 */

/* New algorithm: rely on the fact that all normal ascii is between 32 and 127
 * Rather than check all bytes for zero,
 * Take one word (4 bytes) call it w1
 * Shift w1 left by one into w1'
 * Or w1 and w1', for all normal ascii bit 6 will be 1, for zero it won't
 * Check that all 4 bit 6's (one for each byte) are one,
 *  if they are, we are definitely not done
 *  if they are not, we are probably done, but need to check for zero
 */
	.align	4
#if XCHAL_HAVE_LOOPS
.Laligned:
	movi	a11, 0
	movi	a4, MASK0	# mask for byte 0
	movi	a7, MASK4
	loop	a11, .Laligned_done	# loop forever

	l32i	a8, a2, 0	# get word from s1
	l32i	a9, a3, 0	# get word from s2
        slli	a5, a8, 1
	bne	a8, a9, .Lwne
        or	a9, a8, a5
        bnall   a9, a7, .Lprobeq
	addi	a2, a2, 4	# advance s1 pointer
	addi	a3, a3, 4	# advance s2 pointer
.Laligned_done:

#else /* !XCHAL_HAVE_LOOPS */

.Laligned:
	movi	a4, MASK0	# mask for byte 0
	movi	a7, MASK4
	j	.Lfirstword
.Lnextword:
	addi	a2, a2, 4	# advance s1 pointer
	addi	a3, a3, 4	# advance s2 pointer
.Lfirstword:
	l32i	a8, a2, 0	# get word from s1
	l32i	a9, a3, 0	# get word from s2
	slli	a5, a8, 1
	bne	a8, a9, .Lwne
	or	a9, a8, a5
	ball	a9, a7, .Lnextword
#endif /* !XCHAL_HAVE_LOOPS */

	# align	(0 mod 4)
.Lprobeq:
          /* words are probably equal, but check for sure */
          /*  if not, loop over the rest of string using normal algorithm */

	bnone	a8, a4, .Leq	# if byte 0 is zero
	movi	a5, MASK1	# mask for byte 1
	movi	a6, MASK2	# mask for byte 2
	bnone	a8, a5, .Leq	# if byte 1 is zero
	movi	a7, MASK3	# mask for byte 3
	bnone	a8, a6, .Leq	# if byte 2 is zero
	bnone	a8, a7, .Leq	# if byte 3 is zero
	addi.n	a2, a2, 4	# advance s1 pointer
	addi.n	a3, a3, 4	# advance s2 pointer
#if XCHAL_HAVE_LOOPS

	# align (1 mod 4)
	loop    a11, .Leq    	# loop forever

	l32i	a8, a2, 0	# get word from s1
	l32i	a9, a3, 0	# get word from s2
	addi	a2, a2, 4	# advance s1 pointer
	bne	a8, a9, .Lwne
	bnone	a8, a4, .Leq	# if byte 0 is zero
	bnone	a8, a5, .Leq	# if byte 1 is zero
	bnone	a8, a6, .Leq	# if byte 2 is zero
	bnone	a8, a7, .Leq	# if byte 3 is zero
	addi	a3, a3, 4	# advance s2 pointer

#else /* !XCHAL_HAVE_LOOPS */

	j	.Lfirstword2
.Lnextword2:
	addi	a3, a3, 4	# advance s2 pointer
.Lfirstword2:
	l32i	a8, a2, 0	# get word from s1
	l32i	a9, a3, 0	# get word from s2
	addi	a2, a2, 4	# advance s1 pointer
	bne	a8, a9, .Lwne
	bnone	a8, a4, .Leq	# if byte 0 is zero
	bnone	a8, a5, .Leq	# if byte 1 is zero
	bnone	a8, a6, .Leq	# if byte 2 is zero
	bany	a8, a7, .Lnextword2	# if byte 3 is zero
#endif /* !XCHAL_HAVE_LOOPS */

.Leq:	/* words are equal, some byte is zero */
	movi	a2, 0		# return equal
	retw

.Lwne:	/* words are not equal */
	xor	a2, a8, a9	# get word with nonzero in byte that differs
	bany	a2, a4, .Ldiff0	# if byte 0 differs
	movi	a5, MASK1	# mask for byte 1
	bnone	a8, a4, .Leq	# if byte 0 is zero
	bany	a2, a5, .Ldiff1	# if byte 1 differs
	movi	a6, MASK2	# mask for byte 2
	bnone	a8, a5, .Leq	# if byte 1 is zero
	bany	a2, a6, .Ldiff2	# if byte 2 differs
	bnone	a8, a6, .Leq	# if byte 2 is zero
#ifdef __XTENSA_EB__
.Ldiff3:
.Ldiff2:
.Ldiff1:
	/*
	 * byte 0 is equal (at least)
	 * and there is a difference before a zero byte
	 * just subtract words to get return value
	 * the high order equal bytes cancel, leaving room for the sign
	 */
	sub	a2, a8, a9
	retw
.Ldiff0:
	/* need to make room for the sign, so can't subtract whole words */
	extui	a10, a8, 24, 8
	extui	a11, a9, 24, 8
	sub	a2, a10, a11
	retw
#else /* !__XTENSA_EB__ */
	/* little-endian is a little more difficult because can't subtract
	   whole words */
.Ldiff3:
	/* bytes 0-2 are equal, byte 3 is different */
	/* for little-endian need to have a sign bit for the difference */
	extui	a10, a8, 24, 8
	extui	a11, a9, 24, 8
	sub	a2, a10, a11
	retw
.Ldiff0:
	/* byte 0 is different */
	extui	a10, a8, 0, 8
	extui	a11, a9, 0, 8
	sub	a2, a10, a11
	retw
.Ldiff1:
	/* byte 0 is equal, byte 1 is different */
	extui	a10, a8, 8, 8
	extui	a11, a9, 8, 8
	sub	a2, a10, a11
	retw
.Ldiff2:
	/* bytes 0-1 are equal, byte 2 is different */
	extui	a10, a8, 16, 8
	extui	a11, a9, 16, 8
	sub	a2, a10, a11
	retw
#endif /* !__XTENSA_EB */
END(strcmp)
libc_hidden_builtin_def (strcmp)
