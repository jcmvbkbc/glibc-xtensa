/* Copyright (C) 2012-2013 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Chris Zankel <chris@zankel.net>

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

#ifndef	_LINK_H
# error "Never include <bits/link.h> directly; use <link.h> instead."
#endif

/* Registers for entry into PLT.  */
typedef struct La_xtensa_regs
{
  uint32_t lr_reg[6];
} La_xtensa_regs;

/* Return values for calls from PLT.  */
typedef struct La_xtensa_retval
{
  /* Up to six registers can be used for a return value (e.g. small struct). */
  uint32_t lrv_reg[6];
} La_xtensa_retval;


__BEGIN_DECLS

extern ElfW(Addr) la_xtensa_gnu_pltenter (ElfW(Sym) *__sym, unsigned int __ndx,
					  uintptr_t *__refcook,
					  uintptr_t *__defcook,
					  La_xtensa_regs *__regs,
					  unsigned int *__flags,
					  const char *__symname,
					  long int *__framesizep);
extern unsigned int la_xtensa_gnu_pltexit (ElfW(Sym) *__sym, unsigned int __ndx,
					   uintptr_t *__refcook,
					   uintptr_t *__defcook,
					   const La_xtensa_regs *__inregs,
					   La_xtensa_retval *__outregs,
					   const char *__symname);
__END_DECLS
