/* Return backtrace of current program state.  Xtensa version.
   Copyright (C) 2002-2013 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Joe Taylor <joe@tensilica.com>

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <execinfo.h>
#include <signal.h>
#include <frame.h>
#include <sigcontextinfo.h>

/* This is a global variable set at program start time.  It marks the
   highest used stack address.  */
extern void *__libc_stack_end;


/* This implementation assumes a stack layout that matches the defaults
   used by gcc's `__builtin_frame_address' and `__builtin_return_address'
   (FP is the frame pointer register):

	  +-----------------+     +-----------------+
    FP -> | previous FP --------> | previous FP ------>...
	  |                 |     |                 |
	  | return address  |     | return address  |
	  +-----------------+     +-----------------+

  */

extern void __window_spill(void);

int
__backtrace (void **array, int size)
{
  struct layout *base_save_area;
  void *current;
  register void *top_stack __asm__ ("a1");
  int cnt = 0;

  __window_spill();

  /* We skip the call to this function, it makes no sense to record it.  */
  current = top_stack;

  while (cnt < size)
    {
      if (current < top_stack  ||  current > __libc_stack_end)
       /* This means the address is out of range.  Note that for the
	  toplevel we see a frame pointer with value NULL which clearly is
	  out of range.  */
	break;

      /* Backup to the base save area below the frame. */
      base_save_area = (struct layout *)(((int)current) - 16);

      /* Record the return address after masking off the window-size
	 bits.  A zero return address marks the end of the
	 backtrace. */
      array[cnt] = (void *)(((int)base_save_area->return_address) & 0x3fffffff);
      if (array[cnt] == 0)
	break;
      cnt++;

      /* Load the next frame pointer. */
      current = (struct layout *) base_save_area->next;
    }

  return cnt;
}
weak_alias (__backtrace, backtrace)
libc_hidden_def (__backtrace)
