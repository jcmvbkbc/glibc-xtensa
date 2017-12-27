/* Dump registers.
   Copyright (C) 2005-2013 Free Software Foundation, Inc.
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

#include <sys/uio.h>
#include <_itoa.h>

/* We print the register dump in this format:

 A0   XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX
 A8   XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX

      pc       ps       lbeg     lend     lcount   sar
      XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX XXXXXXXX

 We don't print any coprocessor registers (e.g., FPU).  Generic handling of
 coprocessor state on Xtensa processors would be quite complex, and the
 infrastructure for such a job doesn't exist yet.  What we have now is
 adequate for most cases.

 ++ AR32 - AR63 do not exist on some processor configurations and won't be
    printed on those.
*/


static void
hexvalue (unsigned long int value, char *buf, size_t len)
{
  char *cp = _itoa_word (value, buf + len, 16, 0);
  while (cp > buf)
    *--cp = '0';
}

static void
register_dump (int fd, struct sigcontext *ctx)
{

#define MISC_REG_COUNT 8

/* This is just a manual count of what's needed:
     17 calls to ADD_* macros per for loop
     10 for loops (worst case)
     last for loop: 2 + (MISC_REG_COUNT * 2) calls to ADD_* macros
     4 padding for extensions
*/
#define IOVNR ( (17 * 10) + 2 + (MISC_REG_COUNT * 2) + 4 )

  char miscregs[MISC_REG_COUNT][8];
  char aregs[16][8];
  struct iovec iov[IOVNR];
  size_t nr = 0;
  int i;

#define ADD_STRING(str)			\
  iov[nr].iov_base = (char *) str;	\
  iov[nr].iov_len = strlen (str);	\
  ++nr
#define ADD_MEM(str, len)		\
  iov[nr].iov_base = str;		\
  iov[nr].iov_len = len;		\
  ++nr

  /* Generate strings of live-register contents.  */
  for (i = 0; i < 16; i++)
    hexvalue (ctx->sc_a[i], aregs[i], 8);

  /* Generate strings of special-register contents. */
  hexvalue (ctx->sc_pc,		miscregs[0], 8);
  hexvalue (ctx->sc_ps,		miscregs[1], 8);
  hexvalue (ctx->sc_lbeg,	miscregs[4], 8);
  hexvalue (ctx->sc_lend,	miscregs[5], 8);
  hexvalue (ctx->sc_lcount,	miscregs[6], 8);
  hexvalue (ctx->sc_sar,	miscregs[7], 8);

  /* Generate the output.  */
  ADD_STRING ("Register dump:\n\n A0   ");
  for (i = 0; i < 8; i++)
    {
      ADD_MEM (aregs[i], 8);
      ADD_STRING (" ");
    }
  ADD_STRING ("\n A8   ");
  for (i = 8; i < 16; i++)
    {
      ADD_MEM (aregs[i], 8);
      ADD_STRING (" ");
    }

  ADD_STRING ("\n\n      pc       ps       lbeg     lend     lcount   sar\n      ");
  for (i = 0; i < MISC_REG_COUNT; i++)
    {
      ADD_MEM (miscregs[i], 8);
      ADD_STRING (" ");
    }
  ADD_STRING ("\n");

  /* Write the stuff out.  */
  writev (fd, iov, nr);

}


#define REGISTER_DUMP register_dump (fd, ctx)
