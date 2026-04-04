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
	movq	%r8, -56(%rbp)
	movq	%r9, -64(%rbp)
	movq	-56(%rbp), %r10
	movq	-64(%rbp), %r8
	movq	16(%rbp), %r9
	movq	-24(%rbp), %rax
	movq	-32(%rbp), %rdi
	movq	-40(%rbp), %rsi
	movq	-48(%rbp), %rdx
#APP
# 46 "hello.c" 1
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
	.type	syscall1, @function
syscall1:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-16(%rbp), %rsi
	movq	-8(%rbp), %rax
	pushq	$0
	movl	$0, %r9d
	movl	$0, %r8d
	movl	$0, %ecx
	movl	$0, %edx
	movq	%rax, %rdi
	call	syscall
	addq	$8, %rsp
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	syscall1, .-syscall1
	.type	syscall2, @function
syscall2:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$24, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	-24(%rbp), %rdx
	movq	-16(%rbp), %rsi
	movq	-8(%rbp), %rax
	pushq	$0
	movl	$0, %r9d
	movl	$0, %r8d
	movl	$0, %ecx
	movq	%rax, %rdi
	call	syscall
	addq	$8, %rsp
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	syscall2, .-syscall2
	.type	syscall3, @function
syscall3:
.LFB4:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	%rcx, -32(%rbp)
	movq	-32(%rbp), %rcx
	movq	-24(%rbp), %rdx
	movq	-16(%rbp), %rsi
	movq	-8(%rbp), %rax
	pushq	$0
	movl	$0, %r9d
	movl	$0, %r8d
	movq	%rax, %rdi
	call	syscall
	addq	$8, %rsp
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	syscall3, .-syscall3
	.type	syscall6, @function
syscall6:
.LFB5:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	%rcx, -32(%rbp)
	movq	%r8, -40(%rbp)
	movq	%r9, -48(%rbp)
	movq	-48(%rbp), %r8
	movq	-40(%rbp), %rdi
	movq	-32(%rbp), %rcx
	movq	-24(%rbp), %rdx
	movq	-16(%rbp), %rsi
	movq	-8(%rbp), %rax
	pushq	16(%rbp)
	movq	%r8, %r9
	movq	%rdi, %r8
	movq	%rax, %rdi
	call	syscall
	addq	$8, %rsp
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	syscall6, .-syscall6
	.type	sys_print_write, @function
sys_print_write:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$24, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	-16(%rbp), %rdx
	movl	-4(%rbp), %eax
	cltq
	movq	-24(%rbp), %rcx
	movq	%rax, %rsi
	movl	$67671, %edi
	call	syscall3
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	sys_print_write, .-sys_print_write
	.type	sys_fuck_you, @function
sys_fuck_you:
.LFB7:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	$66
	movl	$55, %r9d
	movl	$44, %r8d
	movl	$33, %ecx
	movl	$22, %edx
	movl	$11, %esi
	movl	$67673, %edi
	call	syscall
	addq	$8, %rsp
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	sys_fuck_you, .-sys_fuck_you
	.type	sys_uname, @function
sys_uname:
.LFB8:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$8, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, %rsi
	movl	$63, %edi
	call	syscall1
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	sys_uname, .-sys_uname
	.type	sys_lseek, @function
sys_lseek:
.LFB9:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -8(%rbp)
	movl	-8(%rbp), %eax
	movslq	%eax, %rcx
	movq	-16(%rbp), %rdx
	movl	-4(%rbp), %eax
	cltq
	movq	%rax, %rsi
	movl	$49, %edi
	call	syscall3
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE9:
	.size	sys_lseek, .-sys_lseek
	.type	sys_mmap, @function
sys_mmap:
.LFB10:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$40, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movl	%ecx, -24(%rbp)
	movl	%r8d, -28(%rbp)
	movq	%r9, -40(%rbp)
	movl	-28(%rbp), %eax
	movslq	%eax, %rdi
	movl	-24(%rbp), %eax
	movslq	%eax, %rsi
	movl	-20(%rbp), %eax
	movslq	%eax, %rcx
	movq	-8(%rbp), %rax
	movq	-16(%rbp), %rdx
	pushq	-40(%rbp)
	movq	%rdi, %r9
	movq	%rsi, %r8
	movq	%rax, %rsi
	movl	$9, %edi
	call	syscall6
	addq	$8, %rsp
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE10:
	.size	sys_mmap, .-sys_mmap
	.type	sys_munmap, @function
sys_munmap:
.LFB11:
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
	movq	%rax, %rsi
	movl	$11, %edi
	call	syscall2
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE11:
	.size	sys_munmap, .-sys_munmap
	.type	sys_mprotect, @function
sys_mprotect:
.LFB12:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$24, %rsp
	movq	%rdi, -8(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movl	-20(%rbp), %eax
	movslq	%eax, %rcx
	movq	-8(%rbp), %rax
	movq	-16(%rbp), %rdx
	movq	%rax, %rsi
	movl	$10, %edi
	call	syscall3
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE12:
	.size	sys_mprotect, .-sys_mprotect
	.type	print_str, @function
print_str:
.LFB14:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$24, %rsp
	movq	%rdi, -24(%rbp)
	movq	-24(%rbp), %rax
	movq	%rax, -8(%rbp)
	jmp	.L26
.L27:
	addq	$1, -8(%rbp)
.L26:
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	jne	.L27
	movq	-8(%rbp), %rax
	subq	-24(%rbp), %rax
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	sys_print_write
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE14:
	.size	print_str, .-print_str
	.section	.rodata
.LC0:
	.string	"0"
	.text
	.type	print_int, @function
print_int:
.LFB15:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$64, %rsp
	movl	%edi, -52(%rbp)
	cmpl	$0, -52(%rbp)
	jne	.L29
	movl	$1, %edx
	leaq	.LC0(%rip), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	sys_print_write
	jmp	.L28
.L29:
	movl	$0, -4(%rbp)
	movl	$0, -8(%rbp)
	cmpl	$0, -52(%rbp)
	jns	.L32
	movl	$1, -8(%rbp)
	negl	-52(%rbp)
	jmp	.L32
.L33:
	movl	-52(%rbp), %edx
	movslq	%edx, %rax
	imulq	$1717986919, %rax, %rax
	shrq	$32, %rax
	movl	%eax, %ecx
	sarl	$2, %ecx
	movl	%edx, %eax
	sarl	$31, %eax
	subl	%eax, %ecx
	movl	%ecx, %eax
	sall	$2, %eax
	addl	%ecx, %eax
	addl	%eax, %eax
	movl	%edx, %ecx
	subl	%eax, %ecx
	movl	%ecx, %eax
	leal	48(%rax), %ecx
	movl	-4(%rbp), %eax
	leal	1(%rax), %edx
	movl	%edx, -4(%rbp)
	movl	%ecx, %edx
	cltq
	movb	%dl, -48(%rbp,%rax)
	movl	-52(%rbp), %eax
	movslq	%eax, %rdx
	imulq	$1717986919, %rdx, %rdx
	shrq	$32, %rdx
	movl	%edx, %ecx
	sarl	$2, %ecx
	cltd
	movl	%ecx, %eax
	subl	%edx, %eax
	movl	%eax, -52(%rbp)
.L32:
	cmpl	$0, -52(%rbp)
	jg	.L33
	cmpl	$0, -8(%rbp)
	je	.L34
	movl	-4(%rbp), %eax
	leal	1(%rax), %edx
	movl	%edx, -4(%rbp)
	cltq
	movb	$45, -48(%rbp,%rax)
.L34:
	movl	$0, -12(%rbp)
	jmp	.L35
.L36:
	movl	-12(%rbp), %eax
	cltq
	movzbl	-48(%rbp,%rax), %eax
	movb	%al, -13(%rbp)
	movl	-4(%rbp), %eax
	subl	-12(%rbp), %eax
	subl	$1, %eax
	cltq
	movzbl	-48(%rbp,%rax), %edx
	movl	-12(%rbp), %eax
	cltq
	movb	%dl, -48(%rbp,%rax)
	movl	-4(%rbp), %eax
	subl	-12(%rbp), %eax
	subl	$1, %eax
	cltq
	movzbl	-13(%rbp), %edx
	movb	%dl, -48(%rbp,%rax)
	addl	$1, -12(%rbp)
.L35:
	movl	-4(%rbp), %eax
	movl	%eax, %edx
	shrl	$31, %edx
	addl	%edx, %eax
	sarl	%eax
	cmpl	%eax, -12(%rbp)
	jl	.L36
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	leaq	-48(%rbp), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	sys_print_write
.L28:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE15:
	.size	print_int, .-print_int
	.section	.rodata
.LC1:
	.string	"0x0"
.LC2:
	.string	"0x"
	.text
	.type	print_hex, @function
print_hex:
.LFB16:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$64, %rsp
	movq	%rdi, -56(%rbp)
	cmpq	$0, -56(%rbp)
	jne	.L38
	movl	$3, %edx
	leaq	.LC1(%rip), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	sys_print_write
	jmp	.L37
.L38:
	movl	$0, -4(%rbp)
	jmp	.L40
.L43:
	movq	-56(%rbp), %rax
	andl	$15, %eax
	movl	%eax, -12(%rbp)
	cmpl	$9, -12(%rbp)
	jg	.L41
	movl	-12(%rbp), %eax
	addl	$48, %eax
	movl	%eax, %ecx
	jmp	.L42
.L41:
	movl	-12(%rbp), %eax
	addl	$87, %eax
	movl	%eax, %ecx
.L42:
	movl	-4(%rbp), %eax
	leal	1(%rax), %edx
	movl	%edx, -4(%rbp)
	cltq
	movb	%cl, -32(%rbp,%rax)
	movq	-56(%rbp), %rax
	shrq	$4, %rax
	movq	%rax, -56(%rbp)
.L40:
	cmpq	$0, -56(%rbp)
	jne	.L43
	movl	$2, %edx
	leaq	.LC2(%rip), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	sys_print_write
	movl	-4(%rbp), %eax
	subl	$1, %eax
	movl	%eax, -8(%rbp)
	jmp	.L44
.L45:
	movl	-8(%rbp), %eax
	cltq
	movzbl	-32(%rbp,%rax), %eax
	movb	%al, -33(%rbp)
	leaq	-33(%rbp), %rax
	movl	$1, %edx
	movq	%rax, %rsi
	movl	$1, %edi
	call	sys_print_write
	subl	$1, -8(%rbp)
.L44:
	cmpl	$0, -8(%rbp)
	jns	.L45
.L37:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE16:
	.size	print_hex, .-print_hex
	.type	print_long, @function
print_long:
.LFB17:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$64, %rsp
	movq	%rdi, -56(%rbp)
	cmpq	$0, -56(%rbp)
	jne	.L47
	movl	$1, %edx
	leaq	.LC0(%rip), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	sys_print_write
	jmp	.L46
.L47:
	movl	$0, -4(%rbp)
	movl	$0, -8(%rbp)
	cmpq	$0, -56(%rbp)
	jns	.L50
	movl	$1, -8(%rbp)
	negq	-56(%rbp)
	jmp	.L50
.L51:
	movq	-56(%rbp), %rcx
	movabsq	$7378697629483820647, %rdx
	movq	%rcx, %rax
	imulq	%rdx
	sarq	$2, %rdx
	movq	%rcx, %rax
	sarq	$63, %rax
	subq	%rax, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	addq	%rax, %rax
	subq	%rax, %rcx
	movq	%rcx, %rdx
	movl	%edx, %eax
	leal	48(%rax), %ecx
	movl	-4(%rbp), %eax
	leal	1(%rax), %edx
	movl	%edx, -4(%rbp)
	movl	%ecx, %edx
	cltq
	movb	%dl, -48(%rbp,%rax)
	movq	-56(%rbp), %rcx
	movabsq	$7378697629483820647, %rdx
	movq	%rcx, %rax
	imulq	%rdx
	movq	%rdx, %rax
	sarq	$2, %rax
	sarq	$63, %rcx
	movq	%rcx, %rdx
	subq	%rdx, %rax
	movq	%rax, -56(%rbp)
.L50:
	cmpq	$0, -56(%rbp)
	jg	.L51
	cmpl	$0, -8(%rbp)
	je	.L52
	movl	-4(%rbp), %eax
	leal	1(%rax), %edx
	movl	%edx, -4(%rbp)
	cltq
	movb	$45, -48(%rbp,%rax)
.L52:
	movl	$0, -12(%rbp)
	jmp	.L53
.L54:
	movl	-12(%rbp), %eax
	cltq
	movzbl	-48(%rbp,%rax), %eax
	movb	%al, -13(%rbp)
	movl	-4(%rbp), %eax
	subl	-12(%rbp), %eax
	subl	$1, %eax
	cltq
	movzbl	-48(%rbp,%rax), %edx
	movl	-12(%rbp), %eax
	cltq
	movb	%dl, -48(%rbp,%rax)
	movl	-4(%rbp), %eax
	subl	-12(%rbp), %eax
	subl	$1, %eax
	cltq
	movzbl	-13(%rbp), %edx
	movb	%dl, -48(%rbp,%rax)
	addl	$1, -12(%rbp)
.L53:
	movl	-4(%rbp), %eax
	movl	%eax, %edx
	shrl	$31, %edx
	addl	%edx, %eax
	sarl	%eax
	cmpl	%eax, -12(%rbp)
	jl	.L54
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	leaq	-48(%rbp), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	sys_print_write
.L46:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE17:
	.size	print_long, .-print_long
	.section	.rodata
.LC3:
	.string	"\n=== TEST: sys_uname ===\n"
.LC4:
	.string	"  sysname:   "
.LC5:
	.string	"\n"
.LC6:
	.string	"  nodename:  "
.LC7:
	.string	"  release:   "
.LC8:
	.string	"  version:   "
.LC9:
	.string	"  machine:   "
.LC10:
	.string	"  [PASS] sys_uname succeeded\n"
	.align 8
.LC11:
	.string	"  [FAIL] sys_uname failed with return: "
	.text
	.globl	test_uname
	.type	test_uname, @function
test_uname:
.LFB18:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$336, %rsp
	leaq	.LC3(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	-336(%rbp), %rax
	movq	%rax, %rdi
	call	sys_uname
	movq	%rax, -8(%rbp)
	cmpq	$0, -8(%rbp)
	jne	.L56
	leaq	.LC4(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	-336(%rbp), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	-336(%rbp), %rax
	addq	$65, %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC7(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	-336(%rbp), %rax
	addq	$130, %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC8(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	-336(%rbp), %rax
	addq	$195, %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC9(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	-336(%rbp), %rax
	addq	$260, %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC10(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L58
.L56:
	leaq	.LC11(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	print_long
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L58:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE18:
	.size	test_uname, .-test_uname
	.section	.rodata
.LC12:
	.string	"\n=== TEST: sys_lseek ===\n"
.LC13:
	.string	"  lseek(-1, 100, SEEK_SET) = "
.LC14:
	.string	" (expected: negative/error)\n"
.LC15:
	.string	"  lseek(0, 0, SEEK_CUR) = "
.LC16:
	.string	"  lseek(1, 0, SEEK_CUR) = "
.LC17:
	.string	"  [INFO] lseek tested\n"
	.text
	.globl	test_lseek
	.type	test_lseek, @function
test_lseek:
.LFB19:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	leaq	.LC12(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	$0, %edx
	movl	$100, %esi
	movl	$-1, %edi
	call	sys_lseek
	movq	%rax, -8(%rbp)
	leaq	.LC13(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	print_long
	leaq	.LC14(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	$1, %edx
	movl	$0, %esi
	movl	$0, %edi
	call	sys_lseek
	movq	%rax, -8(%rbp)
	leaq	.LC15(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	print_long
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	$1, %edx
	movl	$0, %esi
	movl	$1, %edi
	call	sys_lseek
	movq	%rax, -8(%rbp)
	leaq	.LC16(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	print_long
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC17(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE19:
	.size	test_lseek, .-test_lseek
	.type	sys_write, @function
sys_write:
.LFB20:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$24, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movq	%rdx, -24(%rbp)
	movq	-16(%rbp), %rdx
	movl	-4(%rbp), %eax
	cltq
	movq	-24(%rbp), %rcx
	movq	%rax, %rsi
	movl	$1, %edi
	call	syscall3
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE20:
	.size	sys_write, .-sys_write
	.section	.rodata
.LC18:
	.string	"\n=== Unix Syscall Tests ===\n"
.LC19:
	.string	"argv["
.LC20:
	.string	"] = "
.LC21:
	.string	"\n=== All Tests Complete ===\n"
	.text
	.globl	main_program
	.type	main_program, @function
main_program:
.LFB22:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	movq	%rsi, -32(%rbp)
	leaq	.LC18(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	$0, -4(%rbp)
	jmp	.L63
.L64:
	leaq	.LC19(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC20(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	movq	-32(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	addl	$1, -4(%rbp)
.L63:
	movl	-4(%rbp), %eax
	cmpl	-20(%rbp), %eax
	jl	.L64
	movl	$0, %eax
	call	test_mmap_anonymous
	movl	$0, %eax
	call	test_mmap_mprotect
	movl	$0, %eax
	call	test_mmap_multi
	movl	$0, %eax
	call	test_uname
	movl	$0, %eax
	call	test_lseek
	leaq	.LC21(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE22:
	.size	main_program, .-main_program
	.section	.rodata
	.align 8
.LC22:
	.string	"\n--- TEST: anonymous mmap ---\n"
	.align 8
.LC23:
	.string	"  [FAIL] mmap returned MAP_FAILED\n"
.LC24:
	.string	"  OK: got mapping at "
.LC25:
	.string	"  [FAIL] byte "
.LC26:
	.string	" is "
.LC27:
	.string	", expected 0\n"
.LC28:
	.string	"  OK: all bytes zeroed\n"
.LC29:
	.string	"  [FAIL] pattern mismatch at "
	.align 8
.LC30:
	.string	"  OK: pattern write/read-back passed\n"
.LC31:
	.string	"  [FAIL] munmap returned "
.LC32:
	.string	"  OK: munmap succeeded\n"
.LC33:
	.string	"  [PASS] anonymous mmap\n"
	.text
	.globl	test_mmap_anonymous
	.type	test_mmap_anonymous, @function
test_mmap_anonymous:
.LFB23:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	leaq	.LC22(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	$0, %r9d
	movl	$-1, %r8d
	movl	$34, %ecx
	movl	$3, %edx
	movl	$8192, %esi
	movl	$0, %edi
	call	sys_mmap
	movq	%rax, -24(%rbp)
	cmpq	$-1, -24(%rbp)
	jne	.L66
	leaq	.LC23(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L65
.L66:
	leaq	.LC24(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	print_hex
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-24(%rbp), %rax
	movq	%rax, -32(%rbp)
	movl	$1, -4(%rbp)
	movl	$0, -8(%rbp)
	jmp	.L68
.L71:
	movl	-8(%rbp), %eax
	movslq	%eax, %rdx
	movq	-32(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	je	.L69
	leaq	.LC25(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-8(%rbp), %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC26(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-8(%rbp), %eax
	movslq	%eax, %rdx
	movq	-32(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	movzbl	%al, %eax
	movq	%rax, %rdi
	call	print_hex
	leaq	.LC27(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	$0, -4(%rbp)
	jmp	.L70
.L69:
	addl	$1, -8(%rbp)
.L68:
	cmpl	$8191, -8(%rbp)
	jle	.L71
.L70:
	cmpl	$0, -4(%rbp)
	je	.L72
	leaq	.LC28(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L72:
	movl	$0, -12(%rbp)
	jmp	.L73
.L74:
	movl	-12(%rbp), %eax
	movslq	%eax, %rdx
	movq	-32(%rbp), %rax
	addq	%rdx, %rax
	movl	-12(%rbp), %edx
	movb	%dl, (%rax)
	addl	$1, -12(%rbp)
.L73:
	cmpl	$8191, -12(%rbp)
	jle	.L74
	movl	$1, -4(%rbp)
	movl	$0, -16(%rbp)
	jmp	.L75
.L78:
	movl	-16(%rbp), %eax
	movslq	%eax, %rdx
	movq	-32(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	movl	-16(%rbp), %edx
	cmpb	%dl, %al
	je	.L76
	leaq	.LC29(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-16(%rbp), %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	$0, -4(%rbp)
	jmp	.L77
.L76:
	addl	$1, -16(%rbp)
.L75:
	cmpl	$8191, -16(%rbp)
	jle	.L78
.L77:
	cmpl	$0, -4(%rbp)
	je	.L79
	leaq	.LC30(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L79:
	movq	-24(%rbp), %rax
	movl	$8192, %esi
	movq	%rax, %rdi
	call	sys_munmap
	movq	%rax, -40(%rbp)
	cmpq	$0, -40(%rbp)
	je	.L80
	leaq	.LC31(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-40(%rbp), %rax
	movq	%rax, %rdi
	call	print_long
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L65
.L80:
	leaq	.LC32(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC33(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L65:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE23:
	.size	test_mmap_anonymous, .-test_mmap_anonymous
	.section	.rodata
.LC34:
	.string	"\n--- TEST: mprotect ---\n"
.LC35:
	.string	"  [FAIL] mmap failed\n"
	.align 8
.LC36:
	.string	"  [FAIL] mprotect PROT_READ returned "
	.align 8
.LC37:
	.string	"  OK: mprotect PROT_READ applied\n"
	.align 8
.LC38:
	.string	"  [FAIL] data not readable after mprotect\n"
.LC39:
	.string	"  OK: data still readable\n"
	.align 8
.LC40:
	.string	"  [FAIL] mprotect restore returned "
	.align 8
.LC41:
	.string	"  OK: mprotect restore applied\n"
	.align 8
.LC42:
	.string	"  [FAIL] write after mprotect restore failed\n"
	.align 8
.LC43:
	.string	"  OK: write after restore succeeded\n"
.LC44:
	.string	"  [PASS] mprotect\n"
	.text
	.globl	test_mmap_mprotect
	.type	test_mmap_mprotect, @function
test_mmap_mprotect:
.LFB24:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	leaq	.LC34(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	$0, %r9d
	movl	$-1, %r8d
	movl	$34, %ecx
	movl	$3, %edx
	movl	$4096, %esi
	movl	$0, %edi
	call	sys_mmap
	movq	%rax, -8(%rbp)
	cmpq	$-1, -8(%rbp)
	jne	.L82
	leaq	.LC35(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L81
.L82:
	movq	-8(%rbp), %rax
	movb	$-66, (%rax)
	movq	-8(%rbp), %rax
	movl	$1, %edx
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_mprotect
	movq	%rax, -16(%rbp)
	cmpq	$0, -16(%rbp)
	je	.L84
	leaq	.LC36(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	print_long
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_munmap
	jmp	.L81
.L84:
	leaq	.LC37(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	cmpb	$-66, %al
	je	.L85
	leaq	.LC38(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_munmap
	jmp	.L81
.L85:
	leaq	.LC39(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movl	$3, %edx
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_mprotect
	movq	%rax, -16(%rbp)
	cmpq	$0, -16(%rbp)
	je	.L86
	leaq	.LC40(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	print_long
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_munmap
	jmp	.L81
.L86:
	leaq	.LC41(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movb	$-17, (%rax)
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	cmpb	$-17, %al
	je	.L87
	leaq	.LC42(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_munmap
	jmp	.L81
.L87:
	leaq	.LC43(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_munmap
	leaq	.LC44(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L81:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE24:
	.size	test_mmap_mprotect, .-test_mmap_mprotect
	.section	.rodata
	.align 8
.LC45:
	.string	"\n--- TEST: multiple mappings ---\n"
	.align 8
.LC46:
	.string	"  [FAIL] one of the mappings failed\n"
.LC47:
	.string	"  OK: 3 mappings created\n"
.LC48:
	.string	"  [FAIL] mappings overlap\n"
.LC49:
	.string	"  a = "
.LC50:
	.string	"  b = "
.LC51:
	.string	"  c = "
.LC52:
	.string	"  OK: distinct addresses\n"
.LC53:
	.string	"  [PASS] multiple mappings\n"
	.text
	.globl	test_mmap_multi
	.type	test_mmap_multi, @function
test_mmap_multi:
.LFB25:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	leaq	.LC45(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	$0, %r9d
	movl	$-1, %r8d
	movl	$34, %ecx
	movl	$3, %edx
	movl	$4096, %esi
	movl	$0, %edi
	call	sys_mmap
	movq	%rax, -8(%rbp)
	movl	$0, %r9d
	movl	$-1, %r8d
	movl	$34, %ecx
	movl	$3, %edx
	movl	$8192, %esi
	movl	$0, %edi
	call	sys_mmap
	movq	%rax, -16(%rbp)
	movl	$0, %r9d
	movl	$-1, %r8d
	movl	$34, %ecx
	movl	$1, %edx
	movl	$16384, %esi
	movl	$0, %edi
	call	sys_mmap
	movq	%rax, -24(%rbp)
	cmpq	$-1, -8(%rbp)
	je	.L89
	cmpq	$-1, -16(%rbp)
	je	.L89
	cmpq	$-1, -24(%rbp)
	jne	.L90
.L89:
	leaq	.LC46(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	cmpq	$-1, -8(%rbp)
	je	.L91
	movq	-8(%rbp), %rax
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_munmap
.L91:
	cmpq	$-1, -16(%rbp)
	je	.L92
	movq	-16(%rbp), %rax
	movl	$8192, %esi
	movq	%rax, %rdi
	call	sys_munmap
.L92:
	cmpq	$-1, -24(%rbp)
	je	.L98
	movq	-24(%rbp), %rax
	movl	$16384, %esi
	movq	%rax, %rdi
	call	sys_munmap
	jmp	.L98
.L90:
	leaq	.LC47(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	cmpq	-16(%rbp), %rax
	je	.L95
	movq	-16(%rbp), %rax
	cmpq	-24(%rbp), %rax
	je	.L95
	movq	-8(%rbp), %rax
	cmpq	-24(%rbp), %rax
	jne	.L96
.L95:
	leaq	.LC48(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L97
.L96:
	leaq	.LC49(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	print_hex
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC50(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	print_hex
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC51(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	print_hex
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC52(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L97:
	movq	-8(%rbp), %rax
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_munmap
	movq	-16(%rbp), %rax
	movl	$8192, %esi
	movq	%rax, %rdi
	call	sys_munmap
	movq	-24(%rbp), %rax
	movl	$16384, %esi
	movq	%rax, %rdi
	call	sys_munmap
	leaq	.LC53(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L88
.L98:
	nop
.L88:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE25:
	.size	test_mmap_multi, .-test_mmap_multi
	.section	.rodata
.LC54:
	.string	"HELLO VIA SYS_WRITE\n"
	.text
	.globl	test_console_sys_write
	.type	test_console_sys_write, @function
test_console_sys_write:
.LFB26:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	leaq	.LC54(%rip), %rax
	movq	%rax, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	$21, %edx
	movq	%rax, %rsi
	movl	$1, %edi
	call	sys_write
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE26:
	.size	test_console_sys_write, .-test_console_sys_write
	.section	.rodata
.LC55:
	.string	"\nEnvironment:\n"
.LC56:
	.string	"  <none>\n"
.LC57:
	.string	"  envp["
.LC58:
	.string	"INSIDE "
.LC59:
	.string	"\nDone.\n"
	.text
	.globl	_start
	.type	_start, @function
_start:
.LFB27:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$80, %rsp
	movl	%edi, -52(%rbp)
	movq	%rsi, -64(%rbp)
	movq	%rdx, -72(%rbp)
	movabsq	$4908972450905875784, %rax
	movabsq	$2315223284149546863, %rdx
	movq	%rax, -48(%rbp)
	movq	%rdx, -40(%rbp)
	movabsq	$8320773185467936341, %rax
	movabsq	$8315144785244217699, %rdx
	movq	%rax, -32(%rbp)
	movq	%rdx, -24(%rbp)
	movabsq	$7225216861710144544, %rax
	movabsq	$2851507966407785, %rdx
	movq	%rax, -20(%rbp)
	movq	%rdx, -12(%rbp)
	leaq	-48(%rbp), %rax
	movl	$43, %edx
	movq	%rax, %rsi
	movl	$1, %edi
	call	sys_print_write
	leaq	.LC55(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	cmpq	$0, -72(%rbp)
	je	.L101
	movq	-72(%rbp), %rax
	movq	(%rax), %rax
	testq	%rax, %rax
	jne	.L102
.L101:
	leaq	.LC56(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L103
.L102:
	movl	$0, -4(%rbp)
	jmp	.L104
.L105:
	leaq	.LC57(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC58(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC20(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	movq	-72(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	addl	$1, -4(%rbp)
.L104:
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	movq	-72(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	testq	%rax, %rax
	jne	.L105
.L103:
	movq	-64(%rbp), %rdx
	movl	-52(%rbp), %eax
	movq	%rdx, %rsi
	movl	%eax, %edi
	call	main_program
	movl	$0, %eax
	call	test_console_sys_write
	call	sys_fuck_you
	leaq	.LC59(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE27:
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
