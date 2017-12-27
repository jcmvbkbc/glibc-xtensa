/* Copyright (C) 2005-2013 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Joe Taylor <joe@tensilica.com>

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

/* Also see register-dump.h, where we spill live registers to the
 * stack so that we can trace the stack backward. */

#define SIGCONTEXT unsigned long _info, ucontext_t *
#define SIGCONTEXT_EXTRA_ARGS _info,

/* ANDing with 0x3fffffff clears the window-size bits.
   Assumes TASK_SIZE = 0x40000000 */

#define GET_PC(ctx)	((void *) (ctx->uc_mcontext.sc_pc & 0x3fffffff))
#define GET_FRAME(ctx)	((void *) ctx->uc_mcontext.sc_a[1])
#define GET_STACK(ctx)	((void *) ctx->uc_mcontext.sc_a[1])
#define CALL_SIGHANDLER(handler, signo, ctx) \
  (handler)((signo), SIGCONTEXT_EXTRA_ARGS (ctx))

