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
# 39 "hello.c" 1
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
	movq	-40(%rbp), %rsi
	movl	-28(%rbp), %eax
	movslq	%eax, %r8
	movl	-24(%rbp), %eax
	movslq	%eax, %rdi
	movl	-20(%rbp), %eax
	movslq	%eax, %rcx
	movq	-8(%rbp), %rax
	movq	-16(%rbp), %rdx
	pushq	%rsi
	movq	%r8, %r9
	movq	%rdi, %r8
	movq	%rax, %rsi
	movl	$214, %edi
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
	movl	$215, %edi
	call	syscall2
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE11:
	.size	sys_munmap, .-sys_munmap
	.type	print_str, @function
print_str:
.LFB12:
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
	jmp	.L24
.L25:
	addq	$1, -8(%rbp)
.L24:
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	jne	.L25
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
.LFE12:
	.size	print_str, .-print_str
	.section	.rodata
.LC0:
	.string	"0"
	.text
	.type	print_int, @function
print_int:
.LFB13:
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
	jne	.L27
	movl	$1, %edx
	leaq	.LC0(%rip), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	sys_print_write
	jmp	.L26
.L27:
	movl	$0, -4(%rbp)
	movl	$0, -8(%rbp)
	cmpl	$0, -52(%rbp)
	jns	.L30
	movl	$1, -8(%rbp)
	negl	-52(%rbp)
	jmp	.L30
.L31:
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
.L30:
	cmpl	$0, -52(%rbp)
	jg	.L31
	cmpl	$0, -8(%rbp)
	je	.L32
	movl	-4(%rbp), %eax
	leal	1(%rax), %edx
	movl	%edx, -4(%rbp)
	cltq
	movb	$45, -48(%rbp,%rax)
.L32:
	movl	$0, -12(%rbp)
	jmp	.L33
.L34:
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
.L33:
	movl	-4(%rbp), %eax
	movl	%eax, %edx
	shrl	$31, %edx
	addl	%edx, %eax
	sarl	%eax
	cmpl	%eax, -12(%rbp)
	jl	.L34
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	leaq	-48(%rbp), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	sys_print_write
.L26:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE13:
	.size	print_int, .-print_int
	.section	.rodata
.LC1:
	.string	"0x0"
.LC2:
	.string	"0x"
	.text
	.type	print_hex, @function
print_hex:
.LFB14:
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
	jne	.L36
	movl	$3, %edx
	leaq	.LC1(%rip), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	sys_print_write
	jmp	.L35
.L36:
	movl	$0, -4(%rbp)
	jmp	.L38
.L41:
	movq	-56(%rbp), %rax
	andl	$15, %eax
	movl	%eax, -12(%rbp)
	cmpl	$9, -12(%rbp)
	jg	.L39
	movl	-12(%rbp), %eax
	addl	$48, %eax
	movl	%eax, %ecx
	jmp	.L40
.L39:
	movl	-12(%rbp), %eax
	addl	$87, %eax
	movl	%eax, %ecx
.L40:
	movl	-4(%rbp), %eax
	leal	1(%rax), %edx
	movl	%edx, -4(%rbp)
	cltq
	movb	%cl, -32(%rbp,%rax)
	movq	-56(%rbp), %rax
	shrq	$4, %rax
	movq	%rax, -56(%rbp)
.L38:
	cmpq	$0, -56(%rbp)
	jne	.L41
	movl	$2, %edx
	leaq	.LC2(%rip), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	sys_print_write
	movl	-4(%rbp), %eax
	subl	$1, %eax
	movl	%eax, -8(%rbp)
	jmp	.L42
.L43:
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
.L42:
	cmpl	$0, -8(%rbp)
	jns	.L43
.L35:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE14:
	.size	print_hex, .-print_hex
	.type	print_long, @function
print_long:
.LFB15:
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
	jne	.L45
	movl	$1, %edx
	leaq	.LC0(%rip), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	sys_print_write
	jmp	.L44
.L45:
	movl	$0, -4(%rbp)
	movl	$0, -8(%rbp)
	cmpq	$0, -56(%rbp)
	jns	.L48
	movl	$1, -8(%rbp)
	negq	-56(%rbp)
	jmp	.L48
.L49:
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
.L48:
	cmpq	$0, -56(%rbp)
	jg	.L49
	cmpl	$0, -8(%rbp)
	je	.L50
	movl	-4(%rbp), %eax
	leal	1(%rax), %edx
	movl	%edx, -4(%rbp)
	cltq
	movb	$45, -48(%rbp,%rax)
.L50:
	movl	$0, -12(%rbp)
	jmp	.L51
.L52:
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
.L51:
	movl	-4(%rbp), %eax
	movl	%eax, %edx
	shrl	$31, %edx
	addl	%edx, %eax
	sarl	%eax
	cmpl	%eax, -12(%rbp)
	jl	.L52
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	leaq	-48(%rbp), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	sys_print_write
.L44:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE15:
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
.LFB16:
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
	jne	.L54
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
	jmp	.L56
.L54:
	leaq	.LC11(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movq	%rax, %rdi
	call	print_long
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L56:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE16:
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
.LFB17:
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
.LFE17:
	.size	test_lseek, .-test_lseek
	.section	.rodata
	.align 8
.LC18:
	.string	"\n=== TEST: sys_mmap (anonymous) ===\n"
	.align 8
.LC19:
	.string	"  Test 1: Basic anonymous mapping (4KB, RW)\n"
	.align 8
.LC20:
	.string	"    [FAIL] mmap returned NULL/-1\n"
.LC21:
	.string	"    [PASS] mmap returned: "
.LC22:
	.string	"    Read back: "
.LC23:
	.string	", "
	.align 8
.LC24:
	.string	"    [PASS] Memory verification OK\n"
	.align 8
.LC25:
	.string	"    [FAIL] Memory verification FAILED\n"
.LC26:
	.string	"    munmap returned: "
	.align 8
.LC27:
	.string	"\n  Test 2: Larger mapping (8KB)\n"
.LC28:
	.string	"    [PASS] 8KB pattern OK\n"
.LC29:
	.string	"    [FAIL] Pattern FAILED\n"
.LC30:
	.string	"\n  Test 3: Multiple mappings\n"
.LC31:
	.string	"    m1="
.LC32:
	.string	"    m2="
.LC33:
	.string	"    m3="
.LC34:
	.string	"    Values: "
	.text
	.globl	test_mmap_anonymous
	.type	test_mmap_anonymous, @function
test_mmap_anonymous:
.LFB18:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$80, %rsp
	leaq	.LC18(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC19(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	$0, %r9d
	movl	$-1, %r8d
	movl	$34, %ecx
	movl	$3, %edx
	movl	$4096, %esi
	movl	$0, %edi
	call	sys_mmap
	movq	%rax, -24(%rbp)
	cmpq	$-1, -24(%rbp)
	je	.L59
	cmpq	$0, -24(%rbp)
	jne	.L60
.L59:
	leaq	.LC20(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L61
.L60:
	leaq	.LC21(%rip), %rax
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
	movq	-32(%rbp), %rax
	movabsq	$-2401053089206453570, %rcx
	movq	%rcx, (%rax)
	movq	-32(%rbp), %rax
	addq	$8, %rax
	movabsq	$1311768467294899695, %rcx
	movq	%rcx, (%rax)
	leaq	.LC22(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-32(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	print_hex
	leaq	.LC23(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-32(%rbp), %rax
	addq	$8, %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	print_hex
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-32(%rbp), %rax
	movq	(%rax), %rdx
	movabsq	$-2401053089206453570, %rax
	cmpq	%rax, %rdx
	jne	.L62
	movq	-32(%rbp), %rax
	addq	$8, %rax
	movq	(%rax), %rdx
	movabsq	$1311768467294899695, %rax
	cmpq	%rax, %rdx
	jne	.L62
	leaq	.LC24(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L63
.L62:
	leaq	.LC25(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L63:
	movq	-24(%rbp), %rax
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_munmap
	movq	%rax, -40(%rbp)
	leaq	.LC26(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-40(%rbp), %rax
	movq	%rax, %rdi
	call	print_long
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L61:
	leaq	.LC27(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	$0, %r9d
	movl	$-1, %r8d
	movl	$34, %ecx
	movl	$3, %edx
	movl	$8192, %esi
	movl	$0, %edi
	call	sys_mmap
	movq	%rax, -48(%rbp)
	cmpq	$-1, -48(%rbp)
	je	.L64
	cmpq	$0, -48(%rbp)
	jne	.L65
.L64:
	leaq	.LC20(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L66
.L65:
	leaq	.LC21(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-48(%rbp), %rax
	movq	%rax, %rdi
	call	print_hex
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-48(%rbp), %rax
	movq	%rax, -56(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L67
.L68:
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	-56(%rbp), %rax
	addq	%rdx, %rax
	movl	-4(%rbp), %edx
	movb	%dl, (%rax)
	addl	$1, -4(%rbp)
.L67:
	cmpl	$8191, -4(%rbp)
	jle	.L68
	movl	$1, -8(%rbp)
	movl	$0, -12(%rbp)
	jmp	.L69
.L72:
	movl	-12(%rbp), %eax
	movslq	%eax, %rdx
	movq	-56(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	movl	-12(%rbp), %edx
	cmpb	%dl, %al
	je	.L70
	movl	$0, -8(%rbp)
	jmp	.L71
.L70:
	addl	$1, -12(%rbp)
.L69:
	cmpl	$8191, -12(%rbp)
	jle	.L72
.L71:
	cmpl	$0, -8(%rbp)
	je	.L73
	leaq	.LC28(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L74
.L73:
	leaq	.LC29(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L74:
	movq	-48(%rbp), %rax
	movl	$8192, %esi
	movq	%rax, %rdi
	call	sys_munmap
.L66:
	leaq	.LC30(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	$0, %r9d
	movl	$-1, %r8d
	movl	$34, %ecx
	movl	$3, %edx
	movl	$4096, %esi
	movl	$0, %edi
	call	sys_mmap
	movq	%rax, -64(%rbp)
	movl	$0, %r9d
	movl	$-1, %r8d
	movl	$34, %ecx
	movl	$3, %edx
	movl	$4096, %esi
	movl	$0, %edi
	call	sys_mmap
	movq	%rax, -72(%rbp)
	movl	$0, %r9d
	movl	$-1, %r8d
	movl	$34, %ecx
	movl	$3, %edx
	movl	$4096, %esi
	movl	$0, %edi
	call	sys_mmap
	movq	%rax, -80(%rbp)
	leaq	.LC31(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-64(%rbp), %rax
	movq	%rax, %rdi
	call	print_hex
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC32(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-72(%rbp), %rax
	movq	%rax, %rdi
	call	print_hex
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC33(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-80(%rbp), %rax
	movq	%rax, %rdi
	call	print_hex
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	cmpq	$0, -64(%rbp)
	je	.L75
	cmpq	$-1, -64(%rbp)
	je	.L75
	movq	-64(%rbp), %rax
	movq	$43690, (%rax)
.L75:
	cmpq	$0, -72(%rbp)
	je	.L76
	cmpq	$-1, -72(%rbp)
	je	.L76
	movq	-72(%rbp), %rax
	movq	$48059, (%rax)
.L76:
	cmpq	$0, -80(%rbp)
	je	.L77
	cmpq	$-1, -80(%rbp)
	je	.L77
	movq	-80(%rbp), %rax
	movq	$52428, (%rax)
.L77:
	leaq	.LC34(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	cmpq	$0, -64(%rbp)
	je	.L78
	cmpq	$-1, -64(%rbp)
	je	.L78
	movq	-64(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	print_hex
.L78:
	leaq	.LC23(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	cmpq	$0, -72(%rbp)
	je	.L79
	cmpq	$-1, -72(%rbp)
	je	.L79
	movq	-72(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	print_hex
.L79:
	leaq	.LC23(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	cmpq	$0, -80(%rbp)
	je	.L80
	cmpq	$-1, -80(%rbp)
	je	.L80
	movq	-80(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	print_hex
.L80:
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	cmpq	$0, -64(%rbp)
	je	.L81
	cmpq	$-1, -64(%rbp)
	je	.L81
	movq	-64(%rbp), %rax
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_munmap
.L81:
	cmpq	$0, -72(%rbp)
	je	.L82
	cmpq	$-1, -72(%rbp)
	je	.L82
	movq	-72(%rbp), %rax
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_munmap
.L82:
	cmpq	$0, -80(%rbp)
	je	.L84
	cmpq	$-1, -80(%rbp)
	je	.L84
	movq	-80(%rbp), %rax
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_munmap
.L84:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE18:
	.size	test_mmap_anonymous, .-test_mmap_anonymous
	.section	.rodata
.LC35:
	.string	"\n=== Unix Syscall Tests ===\n"
.LC36:
	.string	"argv["
.LC37:
	.string	"] = "
.LC38:
	.string	"\n=== All Tests Complete ===\n"
	.text
	.globl	main_program
	.type	main_program, @function
main_program:
.LFB19:
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
	leaq	.LC35(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	$0, -4(%rbp)
	jmp	.L86
.L87:
	leaq	.LC36(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC37(%rip), %rax
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
.L86:
	movl	-4(%rbp), %eax
	cmpl	-20(%rbp), %eax
	jl	.L87
	movl	$0, %eax
	call	test_uname
	movl	$0, %eax
	call	test_lseek
	movl	$0, %eax
	call	test_mmap_anonymous
	leaq	.LC38(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE19:
	.size	main_program, .-main_program
	.section	.rodata
.LC39:
	.string	"\nEnvironment:\n"
.LC40:
	.string	"  <none>\n"
.LC41:
	.string	"  envp["
.LC42:
	.string	"\nDone.\n"
	.text
	.globl	_start
	.type	_start, @function
_start:
.LFB20:
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
	movq	-64(%rbp), %rdx
	movl	-52(%rbp), %eax
	movq	%rdx, %rsi
	movl	%eax, %edi
	call	main_program
	leaq	.LC39(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	cmpq	$0, -72(%rbp)
	je	.L89
	movq	-72(%rbp), %rax
	movq	(%rax), %rax
	testq	%rax, %rax
	jne	.L90
.L89:
	leaq	.LC40(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L91
.L90:
	movl	$0, -4(%rbp)
	jmp	.L92
.L93:
	leaq	.LC41(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC37(%rip), %rax
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
.L92:
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	movq	-72(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	testq	%rax, %rax
	jne	.L93
.L91:
	call	sys_fuck_you
	leaq	.LC42(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE20:
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
