/* Copyright (C) 2012-2013 Free Software Foundation, Inc.
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

#ifndef _XTENSA_LDSODEFS_H
#define _XTENSA_LDSODEFS_H 1

#include <elf.h>

struct La_xtensa_regs;
struct La_xtensa_retval;

#define ARCH_PLTENTER_MEMBERS                                           \
  ElfW(Addr) (*xtensa_gnu_pltenter) (ElfW(Sym) *, unsigned int, uintptr_t *, \
                                     uintptr_t *, struct La_xtensa_regs *,  \
                                     unsigned int *, const char *,        \
                                     long int *)

#define ARCH_PLTEXIT_MEMBERS \
  ElfW(Addr) (*xtensa_gnu_pltexit) (ElfW(Sym) *, unsigned int, uintptr_t *, \
                                    uintptr_t *, const struct La_xtensa_regs *,\
                                    struct La_xtensa_retval *, const char *)

#include_next <ldsodefs.h>

#endif
