/* Copyright (C) 2005-2013 Free Software Foundation, Inc.

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

/* clone is even more special than fork as it mucks with stacks
   and invokes a function in the right context after its all over.  */

#include <sysdep.h>
#define _ERRNO_H	1
#include <bits/errno.h>
#define __ASSEMBLY__
#include <linux/sched.h>

/* int clone(int (*fn)(void *arg), void *child_stack, int flags, void *arg,
                    a2                    a3               a4        a5
             pid_t *ptid, struct user_desc *tls, pid_t *ctid)
                   a6               a7              16(sp)
*/

        .text
ENTRY (__clone)

	entry	a1, 16

	/* Sanity check arguments.  */
	beqz	a2, L(einval)	/* no NULL function pointers */
	beqz	a3, L(einval)	/* no NULL stack pointers */

	/* a2 and a3 are candidates for destruction by system-call return
	   parameters.  We don't need the stack pointer after the system
	   call.  We trust that the kernel will preserve a6, a7 and a9. */

	mov	a9, a5			/* save function argument */
	mov	a5, a7
	mov	a7, a2			/* save function pointer */
	mov	a8, a6			/* use a8 as a temp */
	mov	a6, a4
	mov	a4, a8
	l32i	a8, a1, 16		/* child_tid */
	movi	a2, SYS_ify(clone)

	/* syscall(NR_clone,clone_flags, usp, parent_tid, child_tls, child_tid)
                     a2         a6        a3        a4        a5         a8
         */

	syscall
	bltz	a2, SYSCALL_ERROR_LABEL
	beqz	a2, L(thread_start)

	/* fall through for parent */

L(pseudo_end):
	retw

L(einval):
	movi	a2, -EINVAL
	j	SYSCALL_ERROR_LABEL

L(thread_start):

	/* start child thread */
	movi	a0, 0			/* terminate the stack frame */
	mov	a6, a9			/* load up the 'arg' parameter */
	callx4	a7			/* call the user's function */

	/* Call _exit.  Note that any return parameter from the user's
	   function in a6 is seen as inputs to _exit. */
#ifdef	PIC
	movi	a2, _exit@PLT
#else
	movi	a2, _exit
#endif
	callx4	a2

PSEUDO_END (__clone)

libc_hidden_def (__clone)
weak_alias (__clone, clone)
