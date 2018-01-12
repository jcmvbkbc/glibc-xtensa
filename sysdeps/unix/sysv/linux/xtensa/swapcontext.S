#include <sysdep.h>
#include <ucontext_i.h>

ENTRY(__swapcontext)
	entry	sp, 16

	call4	__window_spill
	mov	a9, a3
	s32i	a0, a2, MCONTEXT_SC_PC
	addi	a3, a1, -16
	l32i	a4, a3, 0
	l32i	a5, a3, 4
	l32i	a6, a3, 8
	l32i	a7, a3, 12
	s32i	a4, a2, MCONTEXT_SC_A_0 + 0
	s32i	a5, a2, MCONTEXT_SC_A_0 + 4
	s32i	a6, a2, MCONTEXT_SC_A_0 + 8
	s32i	a7, a2, MCONTEXT_SC_A_0 + 12
	extui	a4, a0, 30, 2
	bltui	a4, 2, 1f

	addi	a3, a5, -16
	l32i	a3, a3, 4
	beqi	a4, 3, 2f

	addi	a3, a3, -32
	l32i	a4, a3, 0
	l32i	a5, a3, 4
	l32i	a6, a3, 8
	l32i	a7, a3, 12
	s32i	a4, a2, MCONTEXT_SC_A_0 + 16
	s32i	a5, a2, MCONTEXT_SC_A_0 + 20
	s32i	a6, a2, MCONTEXT_SC_A_0 + 24
	s32i	a7, a2, MCONTEXT_SC_A_0 + 28
	j	1f
2:
	addi	a3, a3, -48
	l32i	a4, a3, 0
	l32i	a5, a3, 4
	l32i	a6, a3, 8
	l32i	a7, a3, 12
	s32i	a4, a2, MCONTEXT_SC_A_0 + 16
	s32i	a5, a2, MCONTEXT_SC_A_0 + 20
	s32i	a6, a2, MCONTEXT_SC_A_0 + 24
	s32i	a7, a2, MCONTEXT_SC_A_0 + 28
	l32i	a4, a3, 16
	l32i	a5, a3, 20
	l32i	a6, a3, 24
	l32i	a7, a3, 28
	s32i	a4, a2, MCONTEXT_SC_A_0 + 32
	s32i	a5, a2, MCONTEXT_SC_A_0 + 36
	s32i	a6, a2, MCONTEXT_SC_A_0 + 40
	s32i	a7, a2, MCONTEXT_SC_A_0 + 44
1:
	movi	a6, SIG_SETMASK
	addi	a7, a9, UCONTEXT_SIGMASK
	addi	a8, a2, UCONTEXT_SIGMASK
	mov	a2, a9
	movi	a4, JUMPTARGET (sigprocmask)
	callx4	a4
	bnez	a6, .Lerror

	l32i	a0, a2, MCONTEXT_SC_PC
	addi	a3, a1, -16
	l32i	a4, a2, MCONTEXT_SC_A_0 + 0
	l32i	a5, a2, MCONTEXT_SC_A_0 + 4
	l32i	a6, a2, MCONTEXT_SC_A_0 + 8
	l32i	a7, a2, MCONTEXT_SC_A_0 + 12
	s32i	a4, a3, 0
	s32i	a5, a3, 4
	s32i	a6, a3, 8
	s32i	a7, a3, 12
	extui	a4, a0, 30, 2
	bltui	a4, 2, 1f

	addi	a3, a5, -16
	l32i	a3, a3, 4
	beqi	a4, 3, 2f

	addi	a3, a3, -32
	l32i	a4, a2, MCONTEXT_SC_A_0 + 16
	l32i	a5, a2, MCONTEXT_SC_A_0 + 20
	l32i	a6, a2, MCONTEXT_SC_A_0 + 24
	l32i	a7, a2, MCONTEXT_SC_A_0 + 28
	s32i	a4, a3, 0
	s32i	a5, a3, 4
	s32i	a6, a3, 8
	s32i	a7, a3, 12
	j	1f
2:
	addi	a3, a3, -48
	l32i	a4, a2, MCONTEXT_SC_A_0 + 16
	l32i	a5, a2, MCONTEXT_SC_A_0 + 20
	l32i	a6, a2, MCONTEXT_SC_A_0 + 24
	l32i	a7, a2, MCONTEXT_SC_A_0 + 28
	s32i	a4, a3, 0
	s32i	a5, a3, 4
	s32i	a6, a3, 8
	s32i	a7, a3, 12
	l32i	a4, a2, MCONTEXT_SC_A_0 + 32
	l32i	a5, a2, MCONTEXT_SC_A_0 + 36
	l32i	a6, a2, MCONTEXT_SC_A_0 + 40
	l32i	a7, a2, MCONTEXT_SC_A_0 + 44
	s32i	a4, a3, 16
	s32i	a5, a3, 20
	s32i	a6, a3, 24
	s32i	a7, a3, 28
1:
	movi	a2, 0
	retw
.Lerror:
	mov	a2, a6
	retw
END(__swapcontext)

weak_alias (__swapcontext, swapcontext)
