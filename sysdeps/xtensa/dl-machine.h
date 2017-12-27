/* Machine-dependent ELF dynamic relocation inline functions.
   Xtensa processor version.
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

#ifndef dl_machine_h
#define dl_machine_h

#define ELF_MACHINE_NAME "Xtensa"

#include <sys/param.h>
#include <tls.h>
#include <dl-tlsdesc.h>


/* Translate a processor specific dynamic tag to the index
   in l_info array.  */
#define DT_XTENSA(x) (DT_XTENSA_##x - DT_LOPROC + DT_NUM)

/* Return nonzero iff ELF header is compatible with the running host.  */
static inline int __attribute__ ((unused))
elf_machine_matches_host (const ElfW(Ehdr) *ehdr)
{
  return ehdr->e_machine == EM_XTENSA;
}

/* Return the link-time address of _DYNAMIC. */
static inline ElfW(Addr)
elf_machine_dynamic (void)
{
  /* This function is only used while bootstrapping the runtime linker.
     The "_DYNAMIC" symbol is always local so its GOT entry will initially
     contain the link-time address. */
  return (ElfW(Addr)) &_DYNAMIC;
}

/* Return the run-time load address of the shared object.  */
static inline ElfW(Addr)
elf_machine_load_address (void)
{
  ElfW(Addr) addr, tmp;
  extern void *here;

  /* At this point, the runtime linker is being bootstrapped and the GOT
     entry used for "&here" will contain the link address.  The CALL0 will
     produce the dynamic address of "here" + 3.  Thus, the end result is
     equal to "dynamic_address(here) - link_address(here)". */

  __asm__ ("\
	mov	%1, a0\n\
here:	_call0	0f\n\
	.align	4\n\
0:	sub	%0, a0, %2\n\
	mov	a0, %1"
	   : "=a" (addr), "=&a" (tmp) : "a" (&here));

  return addr - 3;
}


/* ELF_RTYPE_CLASS_PLT iff TYPE describes relocation of a PLT entry, so
   PLT entries should not be allowed to define the value.  */
#define elf_machine_type_class(type)    \
  (((type) == R_XTENSA_JMP_SLOT) * ELF_RTYPE_CLASS_PLT)

/* A reloc type used for ld.so cmdline arg lookups to reject PLT entries.  */
#define ELF_MACHINE_JMP_SLOT	R_XTENSA_JMP_SLOT

/* Only RELA relocations are used for Xtensa systems. */
#define ELF_MACHINE_NO_REL 1
#define ELF_MACHINE_NO_RELA 0

/* Initial entry point code for the dynamic linker.
   The C function `_dl_start' is the real entry point;
   its return value is the user program's entry point.  */

#define RTLD_START \
  __asm__ ("\
	.text\n\
	.align	4\n\
	.global	_start\n\
	.type	_start, @function\n\
_start:\n\
	# compute load offset in a2: the GOT has not yet been relocated\n\
	# but the entries for local symbols contain the relative offsets\n\
	# and we can explicitly add the load offset in this code\n\
	_call0	0f\n\
	.align	4\n\
0:	movi	a3, _start+3\n\
	sub	a2, a0, a3\n\
	# make sure a0 is cleared to mark the top of stack\n\
	movi	a0, 0\n\
	# user_entry_point = _dl_start(pointer to argument block)\n\
	movi	a4, _dl_start\n\
	mov	a6, sp\n\
	add	a4, a4, a2\n\
	callx4	a4\n\
	/* FALLTHRU */\n\
	.global	_dl_start_user\n\
	.type	_dl_start_user, @function\n\
_dl_start_user:\n\
	# save user_entry_point so we can jump to it\n\
	mov	a3, a6\n\
	# store the highest stack address in __libc_stack_end\n\
	movi	a4, __libc_stack_end\n\
	s32i	sp, a4, 0\n\
	# load _dl_skip_args into a4\n\
	movi	a4, _dl_skip_args\n\
	l32i	a4, a4, 0\n\
	# argc -= _dl_skip_args (with argc @ sp+0)\n\
	l32i	a7, sp, 0\n\
	sub	a7, a7, a4\n\
	s32i	a7, sp, 0\n\
	# shift everything by _dl_skip_args\n\
	addi	a5, sp, 4	# a5 = destination ptr = argv\n\
	add	a4, a5, a4	# a4 = source ptr = argv + _dl_skip_args\n\
	# shift argv\n\
1:	l32i	a6, a4, 0\n\
	addi	a4, a4, 4\n\
	s32i	a6, a5, 0\n\
	addi	a5, a5, 4\n\
	bnez	a6, 1b\n\
	# shift envp\n\
2:	l32i	a6, a4, 0\n\
	addi	a4, a4, 4\n\
	s32i	a6, a5, 0\n\
	addi	a5, a5, 4\n\
	bnez	a6, 2b\n\
	# shift auxiliary table\n\
3:	l32i	a6, a4, 0\n\
	l32i	a8, a4, 4\n\
	addi	a4, a4, 8\n\
	s32i	a6, a5, 0\n\
	s32i	a8, a5, 4\n\
	addi	a5, a5, 8\n\
	bnez	a6, 3b\n\
 	# load _rtld_local._dl_loaded into a6 (1st argument to _dl_init below)\n\
 	movi	a4, _rtld_local\n\
	l32i	a6, a4, 0\n\
 	# call _dl_init (_rtld_local._dl_loaded, argc, argv, envp)\n\
	addi	a8, sp, 4	# argv = sp+4\n\
	addi	a9, a7, 1\n\
	addx4	a9, a9, a8	# envp = argv + (argc+1)*4\n\
	movi	a4, _dl_init\n\
	callx4	a4\n\
	# pass finalizer (_dl_fini) in a2 to the user entry point\n\
	movi	a2, _dl_fini\n\
	# jump to user's entry point (_start)\n\
	jx	a3");

static inline ElfW(Addr)
elf_machine_fixup_plt (struct link_map *map, lookup_t t,
		       const ElfW(Sym) *refsym, const ElfW(Sym) *sym,
		       const ElfW(Rela) *reloc,
		       ElfW(Addr) *reloc_addr, ElfW(Addr) value)
{
  return *reloc_addr = value;
}

/* Return the final value of a plt relocation.  */
static inline ElfW(Addr)
elf_machine_plt_value (struct link_map *map, const ElfW(Rela) *reloc,
		       ElfW(Addr) value)
{
  return value + reloc->r_addend;
}

#endif /* !dl_machine_h */

/* Names of the architecture-specific auditing callback functions.  */
#define ARCH_LA_PLTENTER	xtensa_gnu_pltenter
#define ARCH_LA_PLTEXIT		xtensa_gnu_pltexit

#ifdef RESOLVE_MAP

/* Use the page size from the Xtensa compile-time HAL.  It would be
   nicer to use the page size value passed from the OS in the
   auxiliary vector, but when bootstrapping the runtime linker that
   value is not yet copied into the _dl_pagesize global variable, and
   the "runtime_setup" function below does not have access to the
   auxiliary argument vector.  Besides, if the page size were to
   change, the runtime linker would need to be rebuilt anyway so that
   its segments would be properly aligned.  */

#define XT_PAGESIZE (1 << 12)

#ifndef RTLD_BOOTSTRAP
/* This is defined in rtld.c, but nowhere in the static libc.a; make
   the reference weak so static programs can still link.  This
   declaration cannot be done when compiling rtld.c (i.e. #ifdef
   RTLD_BOOTSTRAP) because rtld.c contains the common defn for
   _dl_rtld_map, which is incompatible with a weak decl in the same
   file.  */
# ifndef SHARED
    weak_extern (_dl_rtld_map);
# endif
#endif

/* Set up the loaded object described by L so its stub functions
   will jump to the on-demand fixup code in dl-runtime.c.  */

typedef struct xtensa_got_location_struct {
  ElfW(Off) offset;
  ElfW(Word) length;
} xtensa_got_location;

auto inline int __attribute__ ((unused, always_inline))
elf_machine_runtime_setup (struct link_map *l, int lazy, int profile)
{
  extern void _dl_runtime_resolve (ElfW(Word));
  extern void _dl_runtime_profile (ElfW(Word));
  ElfW(Addr) rfunc;
  ElfW(Addr) *got;
  int g, got_loc_entries;

  /* Make GOT pages writable. */
  if (l->l_info[DT_XTENSA (GOT_LOC_OFF)] != NULL)
#ifndef RTLD_BOOTSTRAP
    if (l != &GL(dl_rtld_map))
#endif
    {
      xtensa_got_location *got_location;
      typedef int (*mprotect_ptr) (__ptr_t, size_t, int);
      mprotect_ptr mprotect_fn;

#ifndef DL_RO_DYN_SECTION
      l->l_info[DT_XTENSA (GOT_LOC_OFF)]->d_un.d_ptr += l->l_addr;
#endif
      got_location = (xtensa_got_location *)
	D_PTR (l, l_info[DT_XTENSA (GOT_LOC_OFF)]);

      mprotect_fn = &__mprotect;
#ifdef RTLD_BOOTSTRAP
      /* Explicitly add the load address when bootstrapping.  */
      mprotect_fn = (mprotect_ptr) (((char *) mprotect_fn) + l->l_addr);
#endif

      got_loc_entries = (int) l->l_info[DT_XTENSA (GOT_LOC_SZ)]->d_un.d_val;
      for (g = 0; g < got_loc_entries; g++)
	{
	  caddr_t got_start, got_end;
	  int result __attribute__((unused));

	  got_start = ((caddr_t) l->l_addr +
		       (got_location[g].offset & ~(XT_PAGESIZE - 1)));
	  got_end = ((caddr_t) l->l_addr +
		     ((got_location[g].offset + got_location[g].length
		       + XT_PAGESIZE - 1) & ~(XT_PAGESIZE - 1)));
	  /* In case the linker hasn't put the GOT literals on separate
	     pages from code, keep the PROT_EXEC bit set, too.  */
	  result = mprotect_fn (got_start, got_end - got_start,
				PROT_READ|PROT_WRITE|PROT_EXEC);
#ifndef RTLD_BOOTSTRAP
	  if (__builtin_expect (result, 0) < 0)
	    _dl_signal_error (errno, l->l_name, NULL, N_("\
cannot make segment writable for relocation"));
#endif
	}
    }

  /* Decide which runtime-resolver function to use for lazy relocations. */
  if (!profile)
    rfunc = (ElfW(Addr)) &_dl_runtime_resolve;
  else
    {
      rfunc = (ElfW(Addr)) &_dl_runtime_profile;

      if (_dl_name_match_p (GLRO(dl_profile), l))
	GL(dl_profile_map) = l;
    }

#ifdef RTLD_BOOTSTRAP
  /* Explicitly add the load address when bootstrapping.  */
  rfunc += l->l_addr;
#endif

  /* Record the runtime-resolver function in the reserved entry at the
     beginning of the GOT.  The value is never used directly from that
     GOT entry but is grabbed from there when resolving the relocations
     for the GOT. */

  got = (ElfW(Addr) *) D_PTR (l, l_info[DT_PLTGOT]);
  got[0] = rfunc;

  if (l->l_info[ADDRIDX (DT_TLSDESC_GOT)] && lazy)
    *(Elf32_Addr*)(D_PTR (l, l_info[ADDRIDX (DT_TLSDESC_GOT)]) + l->l_addr) =
	(Elf32_Addr) &_dl_tlsdesc_resolve_rela;

  return lazy;
}

/* Perform the relocation specified by RELOC and SYM (which is fully resolved).
   MAP is the object containing the reloc.  */

auto inline void __attribute__((always_inline))
elf_machine_rela (struct link_map *map, const ElfW(Rela) *reloc,
		  const ElfW(Sym) *sym, const struct r_found_version *version,
		  void *const reloc_addr_arg, int skip_ifunc)
{
  ElfW(Addr) *const reloc_addr = reloc_addr_arg;
  const int r_info = ELF32_R_TYPE (reloc->r_info);

  /* Don't use a `switch' here since the address of the jump table
     in the GOT is still in the process of being relocated. */

  if (r_info == R_XTENSA_RELATIVE)
    {
#ifndef RTLD_BOOTSTRAP
      if (map != &GL(dl_rtld_map))
#endif
	*reloc_addr += map->l_addr;
    }
  else if (r_info == R_XTENSA_RTLD)
    {
      if (reloc->r_addend == 1)
	{
	  /* Grab the function ptr stashed at the beginning of the GOT by
	     the runtime_setup function. */
	  *reloc_addr = *(ElfW(Addr) *) D_PTR (map, l_info[DT_PLTGOT]);
	}
      else if (reloc->r_addend == 2)
	{
	  /* Store the link map for the object. */
	  *reloc_addr = (ElfW(Addr)) map;
	}
#ifndef RTLD_BOOTSTRAP
      else
        _dl_reloc_bad_type (map, r_info, 0);
#endif
    }
  else if (r_info != R_XTENSA_NONE)
    {
      struct link_map *sym_map = RESOLVE_MAP (&sym, version, r_info);
      ElfW(Addr) value;

      value = reloc->r_addend;

#ifndef RTLD_BOOTSTRAP
      if (r_info == R_XTENSA_TLS_TPOFF)
	{
	  if (sym != NULL)
	    {
	      CHECK_STATIC_TLS (map, sym_map);
	      *reloc_addr = sym->st_value + sym_map->l_tls_offset 
		+ reloc->r_addend;
	    }
	}
      else if (r_info == R_XTENSA_TLSDESC_FN)
	{
	  if (sym == NULL)
	    *reloc_addr = (ElfW(Addr)) _dl_tlsdesc_undefweak;
	  else
	    {
# ifndef SHARED
	      CHECK_STATIC_TLS (map, sym_map);
# else
	      if (!TRY_STATIC_TLS (map, sym_map))
		*reloc_addr = (ElfW(Addr)) _dl_tlsdesc_dynamic;
	      else
# endif
		*reloc_addr = (ElfW(Addr)) _dl_tlsdesc_return;
	     }
	}
      else if (r_info == R_XTENSA_TLSDESC_ARG)
	{
	  if (sym != NULL)
	    {
# ifndef SHARED
	      CHECK_STATIC_TLS (map, sym_map);
# else
	      if (!TRY_STATIC_TLS (map, sym_map))
	      	*reloc_addr = (ElfW(Addr))
		   _dl_make_tlsdesc_dynamic(sym_map, sym->st_value+*reloc_addr);
	      else
# endif
		*reloc_addr += sym->st_value - sym_map->l_tls_offset;
	    }
	}
      else
#endif
	{
	  if (sym_map)
	    value += sym_map->l_addr + sym->st_value;

	  if (r_info == R_XTENSA_GLOB_DAT || r_info == R_XTENSA_JMP_SLOT)
	    {
	      *reloc_addr = value;
	    }
#ifndef RTLD_BOOTSTRAP
	  else
	    _dl_reloc_bad_type (map, r_info, 0);
#endif
	}
    }
}
auto inline void __attribute__((always_inline))
elf_machine_rela_relative (ElfW(Addr) l_addr, const ElfW(Rela) *reloc,
		           void *const reloc_addr_arg)
{
  ElfW(Addr) *const reloc_addr = reloc_addr_arg;
  *reloc_addr += l_addr;
}
auto void __attribute__((always_inline))
elf_machine_lazy_rel (struct link_map *map,
		      ElfW(Addr) l_addr, const ElfW(Rela) *reloc,
		      int skip_ifunc)
{
  ElfW(Addr) * const reloc_addr = (void *)(l_addr + reloc->r_offset);
  const int r_info = ELF32_R_TYPE (reloc->r_info);

  if (r_info == R_XTENSA_JMP_SLOT)
    {
      /* Perform a RELATIVE reloc on the .got entry that transfers
	 to the stub function. */
      *reloc_addr += l_addr;
    }
  else if (r_info == R_XTENSA_NONE)
    return;
  else
    _dl_reloc_bad_type (map, r_info, 1);
}

#endif /* RESOLVE_MAP */
