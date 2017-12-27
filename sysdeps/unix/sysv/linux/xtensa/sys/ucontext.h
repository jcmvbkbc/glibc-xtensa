/* Copyright (C) 2001, 2006  Free Software Foundation, Inc.
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

/* System V/Xtensa ABI compliant context switching support.  */

#ifndef _SYS_UCONTEXT_H
#define _SYS_UCONTEXT_H	1

#include <features.h>
#include <signal.h>

#include <bits/sigcontext.h>

/*
 * On Xtensa Linux, 'struct sigcontext' and 'ucontext_t' are the same.
 */

typedef struct sigcontext mcontext_t;


/*
 * We have to revise this structure when we add support for coprocessors.
 */

/* Structure to describe FPU registers.  */
typedef struct fpregset
{
  int dummy;
} fpregset_t;


#define MCONTEXT_VERSION 1

/* Userlevel context.  */
typedef struct ucontext
{
  unsigned long int uc_flags;
  struct ucontext *uc_link;
  stack_t uc_stack;
  mcontext_t uc_mcontext;
  sigset_t uc_sigmask;
} ucontext_t;

#endif /* sys/ucontext.h */
