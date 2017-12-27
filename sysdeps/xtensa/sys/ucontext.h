/* Copyright (C) 2001-2013 Free Software Foundation, Inc.
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

/* Type for general register.  */
typedef int greg_t;

/* Number of general registers.  */
#define NGREG	18

/* Container for all general registers.  */
typedef greg_t gregset_t[NGREG];

/* Number of each register is the `gregset_t' array.  */
enum
{
  REG_A0 = 0,
#define REG_A0	REG_A0
  REG_A1 = 1,
#define REG_A1	REG_A1
  REG_A2 = 2,
#define REG_A2	REG_A2
  REG_A3 = 3,
#define REG_A3	REG_A3
  REG_A4 = 4,
#define REG_A4	REG_A4
  REG_A5 = 5,
#define REG_A5	REG_A5
  REG_A6 = 6,
#define REG_A6	REG_A6
  REG_A7 = 7,
#define REG_A7	REG_A7
  REG_A8 = 8,
#define REG_A8	REG_A8
  REG_A9 = 9,
#define REG_A9	REG_A9
  REG_A10 = 10,
#define REG_A10	REG_A10
  REG_A11 = 11,
#define REG_A11	REG_A11
  REG_A12 = 12,
#define REG_A12	REG_A12
  REG_A13 = 13,
#define REG_A13	REG_A13
  REG_A14 = 14,
#define REG_A14	REG_A14
  REG_A15 = 15,
#define REG_A15	REG_A15
};


/* XTFIXME: Base Xtensa Processor configurations don't have an FPU.
   Other code needs this structure, however, so I'm not sure what to
   set it to yet.  Give it four dummy bytes, just to be safe for
   now. */

/* Structure to describe FPU registers.  */
typedef struct fpregset
{
#if 1
	int dummy;
#else   /* what used to populate this structure... */
	int f_pcr;
	int f_psr;
	int f_fpiaddr;
	int f_fpregs[8][3];
#endif
} fpregset_t;


/* XTFIXME: What should go into  this  structure?  For now, just  copy
   m68k; seems harmless. */

/* Context to describe whole processor state.  */
typedef struct
{
  int version;
  gregset_t gregs;
} mcontext_t;

#define MCONTEXT_VERSION 1

/* XTFIXME: What should go into this structure?  For now, remove
   former m68k contents, replace with something known to be
   nonexistant, to flush out all references to this structure. */

/* Userlevel context.  */
typedef struct ucontext
{
	unsigned int xtensa;
#if 0  /* old contents copied from sysdeps/m68k/sys/ucontext.h */
  unsigned long int uc_flags;
  struct ucontext *uc_link;
  __sigset_t uc_sigmask;
  stack_t uc_stack;
  mcontext_t uc_mcontext;
  long int uc_filler[201];
#endif
} ucontext_t;

#endif /* sys/ucontext.h */
