/* Copyright (C) 2005-2013 Free Software Foundation, Inc.
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

/* The following code is only used in the shared library when we
   compile the reentrant version.  Otherwise each system call defines
   each own version.  */

#ifndef	PIC

/* The syscall stubs jump here when they detect an error.  */

#undef CALL_MCOUNT
#define CALL_MCOUNT /* Don't insert the profiling call, it clobbers a2.  */

	.text
ENTRY (__syscall_error)
	entry	a1, 16
	neg	a2, a2
#ifndef _LIBC_REENTRANT
	movi	a3, errno
	s32i	a2, a3, 0
#else
	mov	a6, a2
	movi	a3, __errno_location
	callx4	a3
	s32i	a2, a6, 0
#endif

/* It's faster for the caller to just load up this constant return value.
   See macro PSEUDO in sysdep.h. */
/*	movi	a2, -1 */

	retw
END (__syscall_error)

#endif /* PIC */
