/* Copyright (C) 2013 Free Software Foundation, Inc.
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

#include <tls.h>

/*
  Save the PID value (save the negated value)
  Registers a2, a3 are available, a14, a15 are preserved across
 */
#define SAVE_PID(pid, tp, ar, as)					\
	rur	tp, threadptr;						\
	l32i	pid, tp, PID;						\
	neg	ar, pid;						\
	s32i	pid, tp, PID;						\

/*
  Restore the PID value if we are back in the parent.
 */
#define RESTORE_PID(pid, tp, res)					\
	beqz	res, 1f;						\
	s32i	pid, tp, PID;						\
1:

#define RESTORE_PID12(ar, as, at)					\
	rur	as, threadptr;						\
	l32i	ar, as, PID;						\
	neg	ar, ar;							\
	s32i	ar, as, PID;

#include <sysdeps/unix/sysv/linux/xtensa/vfork.S>
