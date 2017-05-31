	.text
	.file	"Echo.ll"
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:
	pushq	%rax
.Ltmp0:
	.cfi_def_cfa_offset 16
	callq	Echo.main.stringarray
	movl	$1, %eax
	popq	%rcx
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc

	.globl	Echo.main.intint
	.align	16, 0x90
	.type	Echo.main.intint,@function
Echo.main.intint:                       # @Echo.main.intint
	.cfi_startproc
# BB#0:
	movl	$2, c(%rip)
	movl	$1, -4(%rsp)
	movl	$2, -8(%rsp)
	movl	-4(%rsp), %eax
	addl	c(%rip), %eax
	movl	%eax, c(%rip)
	retq
.Lfunc_end1:
	.size	Echo.main.intint, .Lfunc_end1-Echo.main.intint
	.cfi_endproc

	.globl	Echo.main.stringarray
	.align	16, 0x90
	.type	Echo.main.stringarray,@function
Echo.main.stringarray:                  # @Echo.main.stringarray
	.cfi_startproc
# BB#0:
	retq
.Lfunc_end2:
	.size	Echo.main.stringarray, .Lfunc_end2-Echo.main.stringarray
	.cfi_endproc

	.type	c,@object               # @c
	.comm	c,4,4

	.section	".note.GNU-stack","",@progbits
