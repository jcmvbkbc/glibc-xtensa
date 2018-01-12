#include <sysdep.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <ucontext.h>

/*
 *
 *    +------------------ stack top, CFA of __start_context
 * 16 | Outermost caller spill area
 *    +------------------
 * 16 | __start_context overflow area
 *    +------------------
 *    | optional padding
 *    +------------------ Optional arguments 6..argc - 1
 *    | vararg argc - 1
 *    | vararg argc - 2
 *    |  ...
 *    | vararg 6
 *    +------------------ Optional arguments 3..5
 *    | vararg 5
 * 16 | vararg 4
 *    | vararg 3
 *    | padding
 *    +------------------ CFA of pseudo getcontext
 * 16 | Outer caller spill area
 *    +------------------
 * 16 | Spill area of __start_context
 *
 */

void
__makecontext (ucontext_t *ucp, void (*func) (void), int argc, ...)
{
  extern void __start_context (void);
  unsigned long sp = ((unsigned long) ucp->uc_stack.ss_sp
    + ucp->uc_stack.ss_size - 32) & -16;
  unsigned long spill0[4] =
    {
      0, sp + 32, 0, 0,
    };
  va_list ap;
  int i;

  memset ((void *) sp, 0, 32);

  if (argc > 6)
    sp -= 4 * (argc - 2);
  else if (argc > 0)
    sp -= 16;
  sp &= -16;

  ucp->uc_mcontext.sc_pc =
    ((unsigned long) __start_context & 0x3fffffff) + 0x80000000;
  ucp->uc_mcontext.sc_a[0] = 0;
  ucp->uc_mcontext.sc_a[1] = sp;
  ucp->uc_mcontext.sc_a[2] = (unsigned long) func;
  ucp->uc_mcontext.sc_a[3] = (unsigned long) ucp->uc_link;
  ucp->uc_mcontext.sc_a[4] = argc;

  va_start (ap, argc);
  for (i = 0; i < argc; ++i)
    {
      if (i < 3)
	ucp->uc_mcontext.sc_a[5 + i] = va_arg (ap, int);
      else
	((int *) sp)[i - 2] = va_arg (ap, int);
    }
  va_end (ap);

  sp -= 16;
  memcpy ((void *) sp, spill0, sizeof (spill0));
}

weak_alias (__makecontext, makecontext)
