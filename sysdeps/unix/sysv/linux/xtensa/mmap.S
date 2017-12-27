/* Copyright (C) 2005-2013 Free Software Foundation, Inc.
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
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <sysdep.h>

#define PAGE_SHIFT 12

/* This is __ptr_t
   __mmap (__ptr_t addr, size_t len, int prot, int flags, int fd, off_t offset)
                 a2          a3         a4         a5       a6        a7
*/

ENTRY	(__mmap)

	entry	a1, 16

	/* We only support mmap2 in the kernel, so shift offset by page-size.*/

	mov	a8, a6
	mov	a6, a2
	movi	a2, __NR_mmap2
	srli	a9, a7, PAGE_SHIFT

	/* mmap has 6 arguments, which are defined by the ABI as follows:
	 * syscall number	arg0	arg1	arg2	arg3	arg4	arg5
	 * --------------	--------------------------------------------
	 *	a2		 a6	 a3	 a4	 a5	 a8	 a9
	 */

	syscall

	bltz	a2, SYSCALL_ERROR_LABEL
L(pseudo_end):
	retw

PSEUDO_END (__mmap)

weak_alias (__mmap, mmap)
