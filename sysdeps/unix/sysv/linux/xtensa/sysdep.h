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

#ifndef _LINUX_XTENSA_SYSDEP_H
#define _LINUX_XTENSA_SYSDEP_H	1

#include <sysdeps/xtensa/sysdep.h>
#include <sysdeps/unix/sysv/linux/generic/sysdep.h>
#include <sys/syscall.h>

/* Defines RTLD_PRIVATE_ERRNO and USE_DL_SYSINFO.  */
#include <dl-sysdep.h>

#include <tls.h>

/* In order to get __set_errno() definition in INLINE_SYSCALL.  */
#ifndef __ASSEMBLER__
#include <errno.h>
#endif

/* For Linux we can use the system call table in the header file
	/usr/include/asm/unistd.h
   of the kernel.  But these symbols do not follow the SYS_* syntax
   so we have to redefine the `SYS_ify' macro here.  */
#undef SYS_ify
#define SYS_ify(syscall_name)	__NR_##syscall_name

#ifdef __ASSEMBLER__

/* Linux uses a negative return value to indicate syscall errors,
   unlike most Unices, which use the condition codes' carry flag.

   Since version 2.1 the return value of a system call might be
   negative even if the call succeeded.  E.g., the `lseek' system call
   might return a large offset.  Therefore we must not anymore test
   for < 0, but test for a real error by making sure the value in a2
   is a real error number.  Linus said he will make sure the no syscall
   returns a value in -1 .. -4095 as a valid result so we can safely
   test with -4095.  */

# define SYSCALL_ERROR_LABEL .Lsyscall_error

#undef  PSEUDO
#define	PSEUDO(name, syscall_name, args)				      \
    .text;								      \
    ENTRY (name)							      \
	entry	a1, 16;							      \
	DO_CALL	(syscall_name, args);					      \
	movi	a4, -2048;						      \
	movi	a5, 1;							      \
	addx2	a4, a4, a5;						      \
	bgeu	a2, a4, SYSCALL_ERROR_LABEL;				      \
    L(pseudo_end):

#undef  PSEUDO_END
#define PSEUDO_END(name)						      \
    SYSCALL_ERROR_HANDLER;						      \
    END (name)

#undef  PSEUDO_NOERRNO
#define PSEUDO_NOERRNO(name, syscall_name, args)			      \
    .text;								      \
    ENTRY (name)							      \
	entry	a1, 16;							      \
	DO_CALL (syscall_name, args)

#undef  PSEUDO_END_NOERRNO
#define PSEUDO_END_NOERRNO(name)					      \
    END (name)

#undef  ret_NOERRNO
#define ret_NOERRNO	retw

/* The function has to return the error code.  */
#undef  PSEUDO_ERRVAL
#define PSEUDO_ERRVAL(name, syscall_name, args)				      \
    .text;								      \
    ENTRY (name)							      \
	entry	a1, 16;							      \
	DO_CALL (syscall_name, args);					      \
	neg	a2, a2

#undef  PSEUDO_END_ERRVAL
#define PSEUDO_END_ERRVAL(name)						      \
    END (name)

#define ret_ERRVAL retw


#ifndef PIC
/* SYSCALL_ERROR_LABEL is the starting address of a callable function
   defined in a separate file, <sysdep.S>.  This separate module can
   be relocated anywhere, so we take precautions about calling it and
   setting up proper parameters. */
# define SYSCALL_ERROR_HANDLER						      \
SYSCALL_ERROR_LABEL:							      \
	movi	a4, __syscall_error;					      \
	mov	a6, a2;							      \
	callx4	a4;							      \
	movi	a2, -1;							      \
	j	L(pseudo_end)
#else
# if RTLD_PRIVATE_ERRNO
#  define SYSCALL_ERROR_HANDLER						      \
SYSCALL_ERROR_LABEL:							      \
	movi	a4, rtld_errno;						      \
	neg	a2, a2;							      \
	s32i	a2, a4, 0;						      \
	movi	a2, -1;							      \
	j	L(pseudo_end)
# else

#  if IS_IN (libc)
#    define SYSCALL_ERROR_ERRNO __libc_errno
#  else
#    define SYSCALL_ERROR_ERRNO errno
#  endif
#  define SYSCALL_ERROR_HANDLER						      \
SYSCALL_ERROR_LABEL:							      \
	neg	a2, a2;							      \
	mov	a6, a2;							      \
	movi	a4, __errno_location@PLT;				      \
	callx4	a4;						              \
	s32i	a2, a6, 0;						      \
	movi	a2, -1;							      \
	j	L(pseudo_end)
# endif /* RTLD_PRIVATE_ERRNO */
#endif /* PIC */

/* The register layout upon entering the function is:

   return addr	stack ptr	arg0, arg1, arg2, arg3, arg4, arg5
   -----------	---------	----------------------------------
   a0		a1		a2,   a3,   a4,   a5,   a6,   a7

   (Of course a function with say 3 arguments does not have entries for
   arguments 4, 5, and 6.)

   Linux takes system-call arguments in registers.  The ABI and Xtensa
   software conventions require the system-call number in a2.  We move any
   argument that was in a2 to a7, and a7 to a8 if we have all 6 arguments.
   Note that for improved efficiency, we do NOT shift all parameters down
   one register to maintain the original order.

   syscall number		arg0, arg1, arg2, arg3, arg4, arg5
   --------------		----------------------------------
   a2				a6,   a3,   a4,   a5,   a8,   a9

   Upon return, a2 and a3 are clobbered.  All other registers are preserved.
*/

#undef  DO_CALL
#define DO_CALL(syscall_name, nargs)					\
	DO_ARGS_##nargs							\
	movi	a2, SYS_ify (syscall_name);				\
	syscall

#define DO_ARGS_0
#define DO_ARGS_1	mov a6, a2;
#define DO_ARGS_2	mov a6, a2;
#define DO_ARGS_3	mov a6, a2;
#define DO_ARGS_4	mov a6, a2;
#define DO_ARGS_5	mov a8, a6; mov a6, a2;
#define DO_ARGS_6	mov a9, a7; mov a8, a6; mov a6, a2;

#else /* not __ASSEMBLER__ */

#define STR(s) #s
#define LD_ARG(n,ar)    register int _a##n asm (STR(a##n)) = (int) (ar)

#define LD_ARGS_0()
#define LD_ARGS_1(a0)			LD_ARG(6,a0)
#define LD_ARGS_2(a0,a1)		LD_ARGS_1(a0); LD_ARG(3,a1)
#define LD_ARGS_3(a0,a1,a2)		LD_ARGS_2(a0,a1); LD_ARG(4,a2)
#define LD_ARGS_4(a0,a1,a2,a3)		LD_ARGS_3(a0,a1,a2); LD_ARG(5,a3)
#define LD_ARGS_5(a0,a1,a2,a3,a4)	LD_ARGS_4(a0,a1,a2,a3); LD_ARG(8,a4)
#define LD_ARGS_6(a0,a1,a2,a3,a4,a5)	LD_ARGS_5(a0,a1,a2,a3,a4); LD_ARG(9,a5)

#define ASM_ARGS_0			"r"(_a2)
#define ASM_ARGS_1			ASM_ARGS_0, "r"(_a6)
#define ASM_ARGS_2			ASM_ARGS_1, "r"(_a3)
#define ASM_ARGS_3			ASM_ARGS_2, "r"(_a4)
#define ASM_ARGS_4			ASM_ARGS_3, "r"(_a5)
#define ASM_ARGS_5			ASM_ARGS_4, "r"(_a8)
#define ASM_ARGS_6			ASM_ARGS_5, "r"(_a9)

/* Define a macro which expands into the inline wrapper code for a system
   call.  */

#undef INLINE_SYSCALL
#define INLINE_SYSCALL(name, nr, args...)				\
  ({ unsigned long resultvar = INTERNAL_SYSCALL (name, , nr, args);	\
     if (__builtin_expect (INTERNAL_SYSCALL_ERROR_P (resultvar, ), 0))	\
       {								\
         __set_errno (INTERNAL_SYSCALL_ERRNO (resultvar, ));		\
        resultvar = (unsigned long) -1;					\
       }								\
   (long) resultvar; })

#undef INTERNAL_SYSCALL_DECL
#define INTERNAL_SYSCALL_DECL(err) do { } while (0)

#define INTERNAL_SYSCALL_NCS(name, err, nr, args...)			\
  ({ LD_ARG(2, name);							\
     LD_ARGS_##nr(args);						\
     asm volatile ("syscall\n" 						\
	 	   : "=a" (_a2)						\
	 	   : ASM_ARGS_##nr					\
	 	   : "memory");						\
     (long) _a2; })

#undef INTERNAL_SYSCALL
#define INTERNAL_SYSCALL(name, err, nr, args...)			\
  INTERNAL_SYSCALL_NCS (__NR_##name, err, nr, ##args)

#undef INTERNAL_SYSCALL_ERROR_P
#define INTERNAL_SYSCALL_ERROR_P(val, err) 				\
  ((unsigned long) (val) >= -4095L)

#undef INTERNAL_SYSCALL_ERRNO
#define INTERNAL_SYSCALL_ERRNO(val, err)	(-(val))

#endif /* not __ASSEMBLER__ */

#ifdef __ASSEMBLER__
#define	ret		retw
#define	r0		a2	/* Normal return-value register.  */
#define	r1		a3	/* Secondary return-value register.  */
/* Xtensa Processors are:  move dest,src */
#define MOVE(x,y)	mov y, x
#endif

/* Pointer mangling support. */
#if defined NOT_IN_libc && defined IS_IN_rtld
/* We cannot use the thread descriptor because in ld.so we use setjmp
   earlier than the descriptor is initialized.  */
#else
#if 1
# ifdef __ASSEMBLER__
#  define PTR_MANGLE(reg, tmpreg)
# else
#  define PTR_MANGLE(val)
# endif
# define PTR_DEMANGLE(valreg)
#else
# ifdef __ASSEMBLER__
#  define PTR_MANGLE(reg, tmpreg) \
	rur 	tmpreg, threadptr; \
	l32r	tmpreg, tmpreg + POINTER_GUARD; \
	xor	reg, reg, tmpreg
#  define PTR_DEMANGLE(reg, tmpreg) PTR_MANGLE (reg, tmpreg)
# else
#  define PTR_MANGLE(var) \
  (var) = (__typeof (var)) ((uintptr_t) (var) ^ THREAD_GET_POINTER_GUARD())
#  define PTR_DEMANGLE(var)	PTR_MANGLE (var)
# endif
#endif
#endif


#endif /* _XTENSA_SYSDEP_H */
