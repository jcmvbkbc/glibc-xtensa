#include <sysdep.h>

	.literal_position

ENTRY(__start_context)
	mov	a10, a5
	mov	a11, a6
	mov	a12, a7
	bltui	a4, 4, 1f
	l32i	a13, a1, 4
	l32i	a14, a1, 8
	l32i	a15, a1, 12
	addi	a5, a1, 16
	movsp	a1, a5
1:
	callx8	a2
	beqz	a3, 1f
	mov	a6, a3
	movi	a4, JUMPTARGET (setcontext)
	callx4	a4
1:
	movi	a4, JUMPTARGET (_exit)
	callx4	a4
	ill
END(__start_context)
