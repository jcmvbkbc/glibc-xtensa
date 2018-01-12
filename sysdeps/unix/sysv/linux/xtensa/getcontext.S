#include <sysdep.h>
#include <ucontext_i.h>

ENTRY(__getcontext)
	entry	sp, 16

	call4	__window_spill
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
	movi	a6, 0
	movi	a7, 0
	addi	a8, a2, UCONTEXT_SIGMASK
	movi	a4, JUMPTARGET (sigprocmask)
	callx4	a4
	mov	a2, a6
	retw
END(__getcontext)

weak_alias (__getcontext, getcontext)
