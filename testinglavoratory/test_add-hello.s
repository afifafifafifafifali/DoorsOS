	.file	"hello.c"
	.text
	.type	syscall, @function
syscall:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp)
	movq	%rsi, -32(%rbp)
	movq	%rdx, -40(%rbp)
	movq	%rcx, -48(%rbp)
	movq	-24(%rbp), %rax
	movq	-32(%rbp), %rdi
	movq	-40(%rbp), %rsi
	movq	-48(%rbp), %rdx
#APP
# 24 "hello.c" 1
	int $0x80
# 0 "" 2
#NO_APP
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	syscall, .-syscall
	.type	sys_print, @function
sys_print:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-8(%rbp), %rax
	movq	-16(%rbp), %rdx
	movq	%rdx, %rcx
	movq	%rax, %rdx
	movl	$1, %esi
	movl	$1, %edi
	call	syscall
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	sys_print, .-sys_print
	.globl	donald_trump
	.type	donald_trump, @function
donald_trump:
.LFB4:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	$0, -4(%rbp)
	jmp	.L6
.L7:
	movabsq	$6061920192744542020, %rax
	movabsq	$2907406443828562, %rdx
	movq	%rax, -32(%rbp)
	movq	%rdx, -24(%rbp)
	leaq	-32(%rbp), %rax
	movl	$15, %esi
	movq	%rax, %rdi
	call	sys_print
	addl	$1, -4(%rbp)
.L6:
	cmpl	$9, -4(%rbp)
	jle	.L7
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	donald_trump, .-donald_trump
	.globl	_start
	.type	_start, @function
_start:
.LFB5:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movabsq	$4908972450905875784, %rax
	movabsq	$2315223284149546863, %rdx
	movq	%rax, -48(%rbp)
	movq	%rdx, -40(%rbp)
	movabsq	$2327603797202848072, %rax
	movabsq	$729940808559894595, %rdx
	movq	%rax, -32(%rbp)
	movq	%rdx, -24(%rbp)
	movb	$0, -16(%rbp)
	leaq	-48(%rbp), %rax
	movl	$32, %esi
	movq	%rax, %rdi
	call	sys_print
	call	donald_trump
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	_start, .-_start
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
