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
# 107 "hello.c" 1
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
	.type	sys_open, @function
sys_open:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movl	%esi, -12(%rbp)
	movl	%edx, -16(%rbp)
	movl	-16(%rbp), %eax
	movslq	%eax, %rcx
	movl	-12(%rbp), %eax
	movslq	%eax, %rdx
	movq	-8(%rbp), %rax
	movq	%rax, %rsi
	movl	$2, %edi
	call	syscall3
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	sys_open, .-sys_open
	.type	sys_close, @function
sys_close:
.LFB7:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$8, %rsp
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %eax
	cltq
	movq	%rax, %rsi
	movl	$3, %edi
	call	syscall1
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	sys_close, .-sys_close
	.type	sys_read, @function
sys_read:
.LFB8:
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
	movl	$0, %edi
	call	syscall3
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	sys_read, .-sys_read
	.type	sys_write, @function
sys_write:
.LFB9:
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
.LFE9:
	.size	sys_write, .-sys_write
	.type	sys_ioctl, @function
sys_ioctl:
.LFB10:
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
	movl	-4(%rbp), %eax
	cltq
	movq	-24(%rbp), %rcx
	movq	-16(%rbp), %rdx
	movq	%rax, %rsi
	movl	$16, %edi
	call	syscall3
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE10:
	.size	sys_ioctl, .-sys_ioctl
	.type	sys_mmap, @function
sys_mmap:
.LFB11:
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
.LFE11:
	.size	sys_mmap, .-sys_mmap
	.type	sys_munmap, @function
sys_munmap:
.LFB12:
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
.LFE12:
	.size	sys_munmap, .-sys_munmap
	.type	sys_mprotect, @function
sys_mprotect:
.LFB13:
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
.LFE13:
	.size	sys_mprotect, .-sys_mprotect
	.type	sys_print_write, @function
sys_print_write:
.LFB15:
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
.LFE15:
	.size	sys_print_write, .-sys_print_write
	.type	sys_fuck_you, @function
sys_fuck_you:
.LFB16:
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
.LFE16:
	.size	sys_fuck_you, .-sys_fuck_you
	.type	print_str, @function
print_str:
.LFB19:
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
	jmp	.L32
.L33:
	addq	$1, -8(%rbp)
.L32:
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	jne	.L33
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
.LFE19:
	.size	print_str, .-print_str
	.section	.rodata
.LC0:
	.string	"0"
	.text
	.type	print_int, @function
print_int:
.LFB20:
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
	jne	.L35
	movl	$1, %edx
	leaq	.LC0(%rip), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	sys_print_write
	jmp	.L34
.L35:
	movl	$0, -4(%rbp)
	movl	$0, -8(%rbp)
	cmpl	$0, -52(%rbp)
	jns	.L38
	movl	$1, -8(%rbp)
	negl	-52(%rbp)
	jmp	.L38
.L39:
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
.L38:
	cmpl	$0, -52(%rbp)
	jg	.L39
	cmpl	$0, -8(%rbp)
	je	.L40
	movl	-4(%rbp), %eax
	leal	1(%rax), %edx
	movl	%edx, -4(%rbp)
	cltq
	movb	$45, -48(%rbp,%rax)
.L40:
	movl	$0, -12(%rbp)
	jmp	.L41
.L42:
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
.L41:
	movl	-4(%rbp), %eax
	movl	%eax, %edx
	shrl	$31, %edx
	addl	%edx, %eax
	sarl	%eax
	cmpl	%eax, -12(%rbp)
	jl	.L42
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	leaq	-48(%rbp), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	sys_print_write
.L34:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE20:
	.size	print_int, .-print_int
	.type	print_long, @function
print_long:
.LFB21:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movl	%eax, %edi
	call	print_int
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE21:
	.size	print_long, .-print_long
	.section	.rodata
.LC1:
	.string	"0x0"
.LC2:
	.string	"0x"
	.text
	.type	print_hex, @function
print_hex:
.LFB22:
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
	jne	.L46
	movl	$3, %edx
	leaq	.LC1(%rip), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	sys_print_write
	jmp	.L45
.L46:
	movl	$0, -4(%rbp)
	jmp	.L48
.L51:
	movq	-56(%rbp), %rax
	andl	$15, %eax
	movl	%eax, -12(%rbp)
	cmpl	$9, -12(%rbp)
	jg	.L49
	movl	-12(%rbp), %eax
	addl	$48, %eax
	movl	%eax, %ecx
	jmp	.L50
.L49:
	movl	-12(%rbp), %eax
	addl	$87, %eax
	movl	%eax, %ecx
.L50:
	movl	-4(%rbp), %eax
	leal	1(%rax), %edx
	movl	%edx, -4(%rbp)
	cltq
	movb	%cl, -32(%rbp,%rax)
	movq	-56(%rbp), %rax
	shrq	$4, %rax
	movq	%rax, -56(%rbp)
.L48:
	cmpq	$0, -56(%rbp)
	jne	.L51
	movl	$2, %edx
	leaq	.LC2(%rip), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	sys_print_write
	movl	-4(%rbp), %eax
	subl	$1, %eax
	movl	%eax, -8(%rbp)
	jmp	.L52
.L53:
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
.L52:
	cmpl	$0, -8(%rbp)
	jns	.L53
.L45:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE22:
	.size	print_hex, .-print_hex
	.section	.rodata
	.align 8
.LC3:
	.string	"\n--- TEST: /dev/fb0 (framebuffer) ---\n"
.LC4:
	.string	"/dev/fb0"
	.align 8
.LC5:
	.string	"  [FAIL] open /dev/fb0 returned "
.LC6:
	.string	"\n"
.LC7:
	.string	"  OK: opened /dev/fb0 as fd="
	.align 8
.LC8:
	.string	"  [FAIL] ioctl FBIOGET_INFO returned "
	.align 8
.LC9:
	.string	"  OK: ioctl FBIOGET_INFO succeeded\n"
.LC10:
	.string	"  Resolution: "
.LC11:
	.string	"x"
.LC12:
	.string	"  Pitch: "
.LC13:
	.string	" bpp: "
.LC14:
	.string	"  FB address: "
	.align 8
.LC15:
	.string	"  OK: wrote 100x100 red rectangle\n"
.LC16:
	.string	"  Read-back pixel: 0x"
.LC17:
	.string	"  OK: pixel matches red\n"
	.align 8
.LC18:
	.string	"  [WARN] pixel mismatch (may be expected)\n"
.LC19:
	.string	"  [PASS] framebuffer\n"
	.text
	.globl	test_framebuffer
	.type	test_framebuffer, @function
test_framebuffer:
.LFB23:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$496, %rsp
	leaq	.LC3(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	$0, %edx
	movl	$3, %esi
	leaq	.LC4(%rip), %rax
	movq	%rax, %rdi
	call	sys_open
	movl	%eax, -12(%rbp)
	cmpl	$0, -12(%rbp)
	jns	.L56
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-12(%rbp), %eax
	cltq
	movq	%rax, %rdi
	call	print_long
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L55
.L56:
	leaq	.LC7(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-12(%rbp), %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	-80(%rbp), %rdx
	movl	-12(%rbp), %eax
	movl	$17921, %esi
	movl	%eax, %edi
	call	sys_ioctl
	movq	%rax, -24(%rbp)
	cmpq	$0, -24(%rbp)
	je	.L58
	leaq	.LC8(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	print_long
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-12(%rbp), %eax
	movl	%eax, %edi
	call	sys_close
	jmp	.L55
.L58:
	leaq	.LC9(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC10(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-72(%rbp), %rax
	movl	%eax, %edi
	call	print_int
	leaq	.LC11(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-64(%rbp), %rax
	movl	%eax, %edi
	call	print_int
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC12(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-56(%rbp), %rax
	movl	%eax, %edi
	call	print_int
	leaq	.LC13(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movzwl	-48(%rbp), %eax
	movzwl	%ax, %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC14(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-80(%rbp), %rax
	movq	%rax, %rdi
	call	print_hex
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movzwl	-48(%rbp), %eax
	cmpw	$32, %ax
	jne	.L59
	movl	$16711680, -28(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L60
.L61:
	movl	-4(%rbp), %eax
	cltq
	movl	-28(%rbp), %edx
	movl	%edx, -496(%rbp,%rax,4)
	addl	$1, -4(%rbp)
.L60:
	cmpl	$99, -4(%rbp)
	jle	.L61
	movl	$0, -8(%rbp)
	jmp	.L62
.L63:
	leaq	-496(%rbp), %rcx
	movl	-12(%rbp), %eax
	movl	$400, %edx
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	sys_write
	addl	$1, -8(%rbp)
.L62:
	cmpl	$99, -8(%rbp)
	jle	.L63
	leaq	.LC15(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	-84(%rbp), %rcx
	movl	-12(%rbp), %eax
	movl	$4, %edx
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	sys_read
	movq	%rax, -40(%rbp)
	cmpq	$4, -40(%rbp)
	jne	.L59
	leaq	.LC16(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-84(%rbp), %eax
	movl	%eax, %eax
	movq	%rax, %rdi
	call	print_hex
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-84(%rbp), %eax
	cmpl	%eax, -28(%rbp)
	jne	.L65
	leaq	.LC17(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L59
.L65:
	leaq	.LC18(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L59:
	movl	-12(%rbp), %eax
	movl	%eax, %edi
	call	sys_close
	leaq	.LC19(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L55:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE23:
	.size	test_framebuffer, .-test_framebuffer
	.section	.rodata
	.align 8
.LC20:
	.string	"\n--- TEST: /dev/kbio (keyboard events) ---\n"
.LC21:
	.string	"/dev/kbio"
	.align 8
.LC22:
	.string	"  [FAIL] open /dev/kbio returned "
.LC23:
	.string	"  OK: opened /dev/kbio as fd="
.LC24:
	.string	"  OK: got event ("
.LC25:
	.string	" bytes)\n"
.LC26:
	.string	"  scancode: 0x"
.LC27:
	.string	" pressed: "
	.align 8
.LC28:
	.string	"  [INFO] no events pending (EAGAIN = expected)\n"
.LC29:
	.string	"  [PASS] kbio\n"
	.text
	.globl	test_kbio_events
	.type	test_kbio_events, @function
test_kbio_events:
.LFB24:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	leaq	.LC20(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	$0, %edx
	movl	$1, %esi
	leaq	.LC21(%rip), %rax
	movq	%rax, %rdi
	call	sys_open
	movl	%eax, -4(%rbp)
	cmpl	$0, -4(%rbp)
	jns	.L68
	leaq	.LC22(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-4(%rbp), %eax
	cltq
	movq	%rax, %rdi
	call	print_long
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L67
.L68:
	leaq	.LC23(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	-21(%rbp), %rcx
	movl	-4(%rbp), %eax
	movl	$5, %edx
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	sys_read
	movq	%rax, -16(%rbp)
	cmpq	$0, -16(%rbp)
	jle	.L70
	leaq	.LC24(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	print_long
	leaq	.LC25(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC26(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movzbl	-21(%rbp), %eax
	movzbl	%al, %eax
	movq	%rax, %rdi
	call	print_hex
	leaq	.LC27(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movzbl	-20(%rbp), %eax
	movzbl	%al, %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L71
.L70:
	leaq	.LC28(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L71:
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	sys_close
	leaq	.LC29(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L67:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE24:
	.size	test_kbio_events, .-test_kbio_events
	.section	.rodata
	.align 8
.LC30:
	.string	"\n--- TEST: /dev/mouse (mouse events) ---\n"
.LC31:
	.string	"/dev/mouse"
	.align 8
.LC32:
	.string	"  [FAIL] open /dev/mouse returned "
	.align 8
.LC33:
	.string	"  OK: opened /dev/mouse as fd="
.LC34:
	.string	"  OK: got state ("
.LC35:
	.string	"  dx="
.LC36:
	.string	" dy="
.LC37:
	.string	" dz="
.LC38:
	.string	" btn=0x"
	.align 8
.LC39:
	.string	"  [INFO] no mouse state change\n"
.LC40:
	.string	"  [PASS] mouse\n"
	.text
	.globl	test_mouse_events
	.type	test_mouse_events, @function
test_mouse_events:
.LFB25:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	leaq	.LC30(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	$0, %edx
	movl	$1, %esi
	leaq	.LC31(%rip), %rax
	movq	%rax, %rdi
	call	sys_open
	movl	%eax, -4(%rbp)
	cmpl	$0, -4(%rbp)
	jns	.L73
	leaq	.LC32(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-4(%rbp), %eax
	cltq
	movq	%rax, %rdi
	call	print_long
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L72
.L73:
	leaq	.LC33(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	-22(%rbp), %rcx
	movl	-4(%rbp), %eax
	movl	$6, %edx
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	sys_read
	movq	%rax, -16(%rbp)
	cmpq	$0, -16(%rbp)
	jle	.L75
	leaq	.LC34(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	print_long
	leaq	.LC25(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC35(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movzbl	-19(%rbp), %eax
	movsbl	%al, %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC36(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movzbl	-18(%rbp), %eax
	movsbl	%al, %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC37(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movzbl	-17(%rbp), %eax
	movsbl	%al, %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC38(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movzbl	-22(%rbp), %eax
	movzbl	%al, %eax
	movq	%rax, %rdi
	call	print_hex
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L76
.L75:
	leaq	.LC39(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L76:
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	sys_close
	leaq	.LC40(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L72:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE25:
	.size	test_mouse_events, .-test_mouse_events
	.section	.rodata
	.align 8
.LC41:
	.string	"\n--- TEST: anonymous mmap ---\n"
	.align 8
.LC42:
	.string	"  [FAIL] mmap returned MAP_FAILED\n"
.LC43:
	.string	"  OK: got mapping at "
.LC44:
	.string	"  OK: all bytes zeroed\n"
	.align 8
.LC45:
	.string	"  OK: pattern write/read-back passed\n"
.LC46:
	.string	"  [FAIL] munmap failed\n"
.LC47:
	.string	"  OK: munmap succeeded\n"
.LC48:
	.string	"  [PASS] anonymous mmap\n"
	.text
	.globl	test_mmap_anonymous
	.type	test_mmap_anonymous, @function
test_mmap_anonymous:
.LFB26:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	leaq	.LC41(%rip), %rax
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
	jne	.L78
	leaq	.LC42(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L77
.L78:
	leaq	.LC43(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	print_hex
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-24(%rbp), %rax
	movq	%rax, -32(%rbp)
	movl	$1, -4(%rbp)
	movl	$0, -8(%rbp)
	jmp	.L80
.L83:
	movl	-8(%rbp), %eax
	movslq	%eax, %rdx
	movq	-32(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	je	.L81
	movl	$0, -4(%rbp)
	jmp	.L82
.L81:
	addl	$1, -8(%rbp)
.L80:
	cmpl	$8191, -8(%rbp)
	jle	.L83
.L82:
	cmpl	$0, -4(%rbp)
	je	.L84
	leaq	.LC44(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L84:
	movl	$0, -12(%rbp)
	jmp	.L85
.L86:
	movl	-12(%rbp), %eax
	movslq	%eax, %rdx
	movq	-32(%rbp), %rax
	addq	%rdx, %rax
	movl	-12(%rbp), %edx
	movb	%dl, (%rax)
	addl	$1, -12(%rbp)
.L85:
	cmpl	$8191, -12(%rbp)
	jle	.L86
	movl	$1, -4(%rbp)
	movl	$0, -16(%rbp)
	jmp	.L87
.L90:
	movl	-16(%rbp), %eax
	movslq	%eax, %rdx
	movq	-32(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	movl	-16(%rbp), %edx
	cmpb	%dl, %al
	je	.L88
	movl	$0, -4(%rbp)
	jmp	.L89
.L88:
	addl	$1, -16(%rbp)
.L87:
	cmpl	$8191, -16(%rbp)
	jle	.L90
.L89:
	cmpl	$0, -4(%rbp)
	je	.L91
	leaq	.LC45(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L91:
	movq	-24(%rbp), %rax
	movl	$8192, %esi
	movq	%rax, %rdi
	call	sys_munmap
	testq	%rax, %rax
	je	.L92
	leaq	.LC46(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L77
.L92:
	leaq	.LC47(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC48(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L77:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE26:
	.size	test_mmap_anonymous, .-test_mmap_anonymous
	.section	.rodata
.LC49:
	.string	"\n--- TEST: mprotect ---\n"
.LC50:
	.string	"  [FAIL] mmap failed\n"
.LC51:
	.string	"  [FAIL] mprotect PROT_READ\n"
	.align 8
.LC52:
	.string	"  OK: mprotect PROT_READ applied\n"
.LC53:
	.string	"  [FAIL] not readable\n"
.LC54:
	.string	"  OK: data still readable\n"
.LC55:
	.string	"  [FAIL] restore\n"
.LC56:
	.string	"  [FAIL] write after restore\n"
	.align 8
.LC57:
	.string	"  OK: write after restore succeeded\n"
.LC58:
	.string	"  [PASS] mprotect\n"
	.text
	.globl	test_mmap_mprotect
	.type	test_mmap_mprotect, @function
test_mmap_mprotect:
.LFB27:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	leaq	.LC49(%rip), %rax
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
	jne	.L94
	leaq	.LC50(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L93
.L94:
	movq	-8(%rbp), %rax
	movb	$-66, (%rax)
	movq	-8(%rbp), %rax
	movl	$1, %edx
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_mprotect
	testq	%rax, %rax
	je	.L96
	leaq	.LC51(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_munmap
	jmp	.L93
.L96:
	leaq	.LC52(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	cmpb	$-66, %al
	je	.L97
	leaq	.LC53(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_munmap
	jmp	.L93
.L97:
	leaq	.LC54(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movl	$3, %edx
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_mprotect
	testq	%rax, %rax
	je	.L98
	leaq	.LC55(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_munmap
	jmp	.L93
.L98:
	movq	-8(%rbp), %rax
	movb	$-17, (%rax)
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	cmpb	$-17, %al
	je	.L99
	leaq	.LC56(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_munmap
	jmp	.L93
.L99:
	leaq	.LC57(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_munmap
	leaq	.LC58(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L93:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE27:
	.size	test_mmap_mprotect, .-test_mmap_mprotect
	.section	.rodata
	.align 8
.LC59:
	.string	"\n--- TEST: multiple mappings ---\n"
.LC60:
	.string	"  [FAIL] one mapping failed\n"
.LC61:
	.string	"  OK: 3 mappings created\n"
.LC62:
	.string	"  [FAIL] overlap\n"
.LC63:
	.string	"  [PASS] distinct addresses\n"
	.text
	.globl	test_mmap_multi
	.type	test_mmap_multi, @function
test_mmap_multi:
.LFB28:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	leaq	.LC59(%rip), %rax
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
	je	.L101
	cmpq	$-1, -16(%rbp)
	je	.L101
	cmpq	$-1, -24(%rbp)
	jne	.L102
.L101:
	leaq	.LC60(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	cmpq	$-1, -8(%rbp)
	je	.L103
	movq	-8(%rbp), %rax
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_munmap
.L103:
	cmpq	$-1, -16(%rbp)
	je	.L104
	movq	-16(%rbp), %rax
	movl	$8192, %esi
	movq	%rax, %rdi
	call	sys_munmap
.L104:
	cmpq	$-1, -24(%rbp)
	je	.L110
	movq	-24(%rbp), %rax
	movl	$16384, %esi
	movq	%rax, %rdi
	call	sys_munmap
	jmp	.L110
.L102:
	leaq	.LC61(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	cmpq	-16(%rbp), %rax
	je	.L107
	movq	-16(%rbp), %rax
	cmpq	-24(%rbp), %rax
	je	.L107
	movq	-8(%rbp), %rax
	cmpq	-24(%rbp), %rax
	jne	.L108
.L107:
	leaq	.LC62(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L109
.L108:
	leaq	.LC63(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L109:
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
	jmp	.L100
.L110:
	nop
.L100:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE28:
	.size	test_mmap_multi, .-test_mmap_multi
	.section	.rodata
.LC64:
	.string	"\n=== Unix Syscall Tests ===\n"
.LC65:
	.string	"argv["
.LC66:
	.string	"] = "
.LC67:
	.string	"\n=== All Tests Complete ===\n"
	.text
	.globl	main_program
	.type	main_program, @function
main_program:
.LFB29:
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
	leaq	.LC64(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	$0, -4(%rbp)
	jmp	.L112
.L113:
	leaq	.LC65(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC66(%rip), %rax
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
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	addl	$1, -4(%rbp)
.L112:
	movl	-4(%rbp), %eax
	cmpl	-20(%rbp), %eax
	jl	.L113
	movl	$0, %eax
	call	test_mmap_anonymous
	movl	$0, %eax
	call	test_mmap_mprotect
	movl	$0, %eax
	call	test_mmap_multi
	movl	$0, %eax
	call	test_framebuffer
	movl	$0, %eax
	call	test_kbio_events
	movl	$0, %eax
	call	test_mouse_events
	leaq	.LC67(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE29:
	.size	main_program, .-main_program
	.section	.rodata
.LC68:
	.string	"\nEnvironment:\n"
.LC69:
	.string	"  <none>\n"
.LC70:
	.string	"  envp["
.LC71:
	.string	"\nDone.\n"
	.text
	.globl	_start
	.type	_start, @function
_start:
.LFB30:
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
	leaq	.LC68(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	cmpq	$0, -72(%rbp)
	je	.L115
	movq	-72(%rbp), %rax
	movq	(%rax), %rax
	testq	%rax, %rax
	jne	.L116
.L115:
	leaq	.LC69(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L117
.L116:
	movl	$0, -4(%rbp)
	jmp	.L118
.L119:
	leaq	.LC70(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC66(%rip), %rax
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
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	addl	$1, -4(%rbp)
.L118:
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	movq	-72(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	testq	%rax, %rax
	jne	.L119
.L117:
	movq	-64(%rbp), %rdx
	movl	-52(%rbp), %eax
	movq	%rdx, %rsi
	movl	%eax, %edi
	call	main_program
	call	sys_fuck_you
	leaq	.LC71(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE30:
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
