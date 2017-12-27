/* Assembler macros for Xtensa processors.
   Copyright (C) 1998-2013 Free Software Foundation, Inc.
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

#include <sysdeps/generic/sysdep.h>

#if defined(__ASSEMBLER__) || defined(REQUEST_ASSEMBLER_MACROS)

/* Syntactic details of assembler.  Note that we assume ELF.  */


/* ELF uses byte-counts for .align, most others use log2 of count of bytes.  */
#define ALIGNARG(log2) 1<<log2

/* For ELF we need the `.type' directive to make shared libs work right.  */
#define ASM_TYPE_DIRECTIVE(name,typearg) .type name,typearg
#define ASM_SIZE_DIRECTIVE(name) .size name,.-name



/* Define an entry point visible from C.

   There is currently a bug in gdb which prevents us from specifying
   incomplete stabs information.  Fake some entries here which specify
   the current source file.  */
#define	ENTRY(name)							      \
  .globl C_SYMBOL_NAME(name);						      \
  ASM_TYPE_DIRECTIVE (C_SYMBOL_NAME(name),@function);			      \
  .align ALIGNARG(2);							      \
  C_LABEL(name)								      \
  CALL_MCOUNT

#undef END
#define END(name) ASM_SIZE_DIRECTIVE(name)


/* We don't do anything for profiling... yet. */

#define CALL_MCOUNT		/* Do nothing.  */


/* Since C identifiers are not normally prefixed with an underscore
   on this system, the asm identifier `syscall_error' intrudes on the
   C name space.  Make sure we use an innocuous name.  */
#define	syscall_error	__syscall_error
#define mcount		_mcount

/* Local label name for asm code. */
#ifndef L
# ifdef HAVE_ELF
#  define L(name)	.L##name
# else
#  define L(name)	name
# endif
#endif

#endif	/* __ASSEMBLER__ */
