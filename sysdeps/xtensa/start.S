/* Startup code compliant to the ELF Xtensa ABI.
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

/* This is the canonical entry point, usually the first thing in the text
   segment.  When the entry point runs, most register values are unspecified,
   except for:

   a2		Contains a function pointer to be registered with `atexit'.
		This is how the dynamic linker arranges to have DT_FINI
		functions called for shared libraries that have been loaded
		before this code runs.

   a1		The stack (i.e., a1+16) contains the arguments and environment:
		0(sp)			argc
		4(sp)			argv[0]
		...
		(4*argc)(sp)		NULL
		(4*(argc+1))(sp)	envp[0]
		...
					NULL


  Setup parameters accordingly (for a call4). See function prototype
  from sysdeps/generic/libc-start.c

	a6   = *main
	a7   = argc
	a8   = ubp_av
	a9   = *init
	a10  = *fini
	a11  = *rtld_fini
	sp+0 = stack_end
*/


	.text
	.literal_position
	.globl _start
_start:
	/* Clear a0 to mark the outermost frame obvious.  */
	movi	a0, 0

	/* Load up the user's main function. */
	movi	a6, main

	/* Extract the arguments as encoded on the stack and set up
	   the arguments for `main': argc, argv.  envp will be determined
	   later in __libc_start_main.  */
	l32i	a7, a1, 0	/* Load the argument count.  */
	addi	a8, a1, 4	/* Compute the argv pointer. */

	/* Push address of our own entry points to .fini and .init.  */
	movi	a9, __libc_csu_init
	movi	a10, __libc_csu_fini

	/* Setup the shared library termination function. */
	mov	a11, a2

	/* Provide the highest stack address to the user code (for stacks
	   which grow downwards).  Note that we destroy the stack version
	   of argc here. */
	s32i	a1, a1, 0

	/* Call the user's main function, and exit with its value.
	   But let the libc call main.    */
	movi	a4, __libc_start_main
	callx4	a4

__crash:
	j	__crash		/* Crash if somehow `exit' does return.  */


/* Define a symbol for the first piece of initialized data.  */
	.data
	.align	4
	.globl __data_start
__data_start:
	.long 0
	.weak data_start
	data_start = __data_start
