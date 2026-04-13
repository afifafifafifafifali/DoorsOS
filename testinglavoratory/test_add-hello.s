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
# 132 "hello.c" 1
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
	.type	print_hex, @function
print_hex:
.LFB20:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movq	%rdi, -40(%rbp)
	movq	$30768, -32(%rbp)
	movq	$0, -24(%rbp)
	movl	$0, -17(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L35
.L36:
	movl	$15, %eax
	subl	-4(%rbp), %eax
	sall	$2, %eax
	movq	-40(%rbp), %rdx
	movl	%eax, %ecx
	shrq	%cl, %rdx
	movq	%rdx, %rax
	andl	$15, %eax
	movq	%rax, %rdx
	movl	-4(%rbp), %eax
	leal	2(%rax), %ecx
	leaq	hex.0(%rip), %rax
	movzbl	(%rdx,%rax), %edx
	movslq	%ecx, %rax
	movb	%dl, -32(%rbp,%rax)
	addl	$1, -4(%rbp)
.L35:
	cmpl	$15, -4(%rbp)
	jle	.L36
	movb	$0, -14(%rbp)
	leaq	-32(%rbp), %rax
	movl	$18, %edx
	movq	%rax, %rsi
	movl	$1, %edi
	call	sys_print_write
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE20:
	.size	print_hex, .-print_hex
	.section	.rodata
.LC0:
	.string	"AT_NULL"
.LC1:
	.string	"AT_IGNORE"
.LC2:
	.string	"AT_EXECFD"
.LC3:
	.string	"AT_PHDR"
.LC4:
	.string	"AT_PHENT"
.LC5:
	.string	"AT_PHNUM"
.LC6:
	.string	"AT_PAGESZ"
.LC7:
	.string	"AT_BASE"
.LC8:
	.string	"AT_FLAGS"
.LC9:
	.string	"AT_ENTRY"
.LC10:
	.string	"AT_NOTELF"
.LC11:
	.string	"AT_UID"
.LC12:
	.string	"AT_EUID"
.LC13:
	.string	"AT_GID"
.LC14:
	.string	"AT_EGID"
.LC15:
	.string	"AT_CLKTCK"
.LC16:
	.string	"AT_RANDOM"
.LC17:
	.string	"AT_EXECFN"
.LC18:
	.string	"AT_UNKNOWN"
	.text
	.type	auxv_type_to_str, @function
auxv_type_to_str:
.LFB21:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	cmpq	$31, -8(%rbp)
	ja	.L38
	movq	-8(%rbp), %rax
	leaq	0(,%rax,4), %rdx
	leaq	.L40(%rip), %rax
	movl	(%rdx,%rax), %eax
	cltq
	leaq	.L40(%rip), %rdx
	addq	%rdx, %rax
	notrack jmp	*%rax
	.section	.rodata
	.align 4
	.align 4
.L40:
	.long	.L57-.L40
	.long	.L56-.L40
	.long	.L55-.L40
	.long	.L54-.L40
	.long	.L53-.L40
	.long	.L52-.L40
	.long	.L51-.L40
	.long	.L50-.L40
	.long	.L49-.L40
	.long	.L48-.L40
	.long	.L47-.L40
	.long	.L46-.L40
	.long	.L45-.L40
	.long	.L44-.L40
	.long	.L43-.L40
	.long	.L38-.L40
	.long	.L38-.L40
	.long	.L42-.L40
	.long	.L38-.L40
	.long	.L38-.L40
	.long	.L38-.L40
	.long	.L38-.L40
	.long	.L38-.L40
	.long	.L38-.L40
	.long	.L38-.L40
	.long	.L41-.L40
	.long	.L38-.L40
	.long	.L38-.L40
	.long	.L38-.L40
	.long	.L38-.L40
	.long	.L38-.L40
	.long	.L39-.L40
	.text
.L57:
	leaq	.LC0(%rip), %rax
	jmp	.L58
.L56:
	leaq	.LC1(%rip), %rax
	jmp	.L58
.L55:
	leaq	.LC2(%rip), %rax
	jmp	.L58
.L54:
	leaq	.LC3(%rip), %rax
	jmp	.L58
.L53:
	leaq	.LC4(%rip), %rax
	jmp	.L58
.L52:
	leaq	.LC5(%rip), %rax
	jmp	.L58
.L51:
	leaq	.LC6(%rip), %rax
	jmp	.L58
.L50:
	leaq	.LC7(%rip), %rax
	jmp	.L58
.L49:
	leaq	.LC8(%rip), %rax
	jmp	.L58
.L48:
	leaq	.LC9(%rip), %rax
	jmp	.L58
.L47:
	leaq	.LC10(%rip), %rax
	jmp	.L58
.L46:
	leaq	.LC11(%rip), %rax
	jmp	.L58
.L45:
	leaq	.LC12(%rip), %rax
	jmp	.L58
.L44:
	leaq	.LC13(%rip), %rax
	jmp	.L58
.L43:
	leaq	.LC14(%rip), %rax
	jmp	.L58
.L42:
	leaq	.LC15(%rip), %rax
	jmp	.L58
.L41:
	leaq	.LC16(%rip), %rax
	jmp	.L58
.L39:
	leaq	.LC17(%rip), %rax
	jmp	.L58
.L38:
	leaq	.LC18(%rip), %rax
.L58:
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE21:
	.size	auxv_type_to_str, .-auxv_type_to_str
	.section	.rodata
.LC19:
	.string	"\nAuxiliary Vector:\n"
.LC20:
	.string	"  <none>\n"
.LC21:
	.string	"  "
.LC22:
	.string	" ("
.LC23:
	.string	") = "
.LC24:
	.string	"\n"
.LC25:
	.string	"  AT_NULL (end of vector)\n"
	.text
	.type	print_auxv, @function
print_auxv:
.LFB22:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movq	%rdi, -24(%rbp)
	leaq	.LC19(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	cmpq	$0, -24(%rbp)
	jne	.L60
	leaq	.LC20(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L59
.L60:
	movl	$0, -4(%rbp)
	jmp	.L62
.L63:
	leaq	.LC21(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-4(%rbp), %eax
	cltq
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	auxv_type_to_str
	movq	%rax, %rdi
	call	print_str
	leaq	.LC22(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-4(%rbp), %eax
	cltq
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	movl	%eax, %edi
	call	print_int
	leaq	.LC23(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-4(%rbp), %eax
	cltq
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movq	8(%rax), %rax
	movq	%rax, %rdi
	call	print_ulong
	leaq	.LC24(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	addl	$1, -4(%rbp)
.L62:
	movl	-4(%rbp), %eax
	cltq
	salq	$4, %rax
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	testq	%rax, %rax
	jne	.L63
	leaq	.LC25(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L59:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE22:
	.size	print_auxv, .-print_auxv
	.type	print_ulong, @function
print_ulong:
.LFB23:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movq	%rdi, -40(%rbp)
	movl	$21, -4(%rbp)
	movl	-4(%rbp), %eax
	cltq
	movb	$0, -32(%rbp,%rax)
	cmpq	$0, -40(%rbp)
	jne	.L67
	subl	$1, -4(%rbp)
	movl	-4(%rbp), %eax
	cltq
	movb	$48, -32(%rbp,%rax)
	jmp	.L66
.L68:
	movq	-40(%rbp), %rcx
	movabsq	$-3689348814741910323, %rdx
	movq	%rcx, %rax
	mulq	%rdx
	shrq	$3, %rdx
	movq	%rdx, %rax
	salq	$2, %rax
	addq	%rdx, %rax
	addq	%rax, %rax
	subq	%rax, %rcx
	movq	%rcx, %rdx
	movl	%edx, %eax
	addl	$48, %eax
	subl	$1, -4(%rbp)
	movl	%eax, %edx
	movl	-4(%rbp), %eax
	cltq
	movb	%dl, -32(%rbp,%rax)
	movq	-40(%rbp), %rax
	movabsq	$-3689348814741910323, %rdx
	mulq	%rdx
	movq	%rdx, %rax
	shrq	$3, %rax
	movq	%rax, -40(%rbp)
.L67:
	cmpq	$0, -40(%rbp)
	jne	.L68
.L66:
	movl	-4(%rbp), %eax
	cltq
	movl	$21, %edx
	subq	%rax, %rdx
	leaq	-32(%rbp), %rcx
	movl	-4(%rbp), %eax
	cltq
	addq	%rcx, %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	sys_print_write
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE23:
	.size	print_ulong, .-print_ulong
	.section	.rodata
.LC26:
	.string	"0"
	.text
	.type	print_int, @function
print_int:
.LFB24:
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
	jne	.L70
	movl	$1, %edx
	leaq	.LC26(%rip), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	sys_print_write
	jmp	.L69
.L70:
	movl	$0, -4(%rbp)
	movl	$0, -8(%rbp)
	cmpl	$0, -52(%rbp)
	jns	.L73
	movl	$1, -8(%rbp)
	negl	-52(%rbp)
	jmp	.L73
.L74:
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
.L73:
	cmpl	$0, -52(%rbp)
	jg	.L74
	cmpl	$0, -8(%rbp)
	je	.L75
	movl	-4(%rbp), %eax
	leal	1(%rax), %edx
	movl	%edx, -4(%rbp)
	cltq
	movb	$45, -48(%rbp,%rax)
.L75:
	movl	$0, -12(%rbp)
	jmp	.L76
.L77:
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
.L76:
	movl	-4(%rbp), %eax
	movl	%eax, %edx
	shrl	$31, %edx
	addl	%edx, %eax
	sarl	%eax
	cmpl	%eax, -12(%rbp)
	jl	.L77
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	leaq	-48(%rbp), %rax
	movq	%rax, %rsi
	movl	$1, %edi
	call	sys_print_write
.L69:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE24:
	.size	print_int, .-print_int
	.type	print_long, @function
print_long:
.LFB25:
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
.LFE25:
	.size	print_long, .-print_long
	.section	.rodata
	.align 8
.LC27:
	.string	"\n--- TEST: /dev/fb0 (framebuffer) ---\n"
.LC28:
	.string	"/dev/fb0"
	.align 8
.LC29:
	.string	"  [FAIL] open /dev/fb0 returned "
.LC30:
	.string	"  OK: opened /dev/fb0 as fd="
	.align 8
.LC31:
	.string	"  [FAIL] ioctl FBIOGET_INFO returned "
	.align 8
.LC32:
	.string	"  OK: ioctl FBIOGET_INFO succeeded\n"
.LC33:
	.string	"  Resolution: "
.LC34:
	.string	"x"
.LC35:
	.string	"  Pitch: "
.LC36:
	.string	" bpp: "
.LC37:
	.string	"  FB address: "
	.align 8
.LC38:
	.string	"  OK: wrote 100x100 red rectangle\n"
.LC39:
	.string	"  Read-back pixel: 0x"
.LC40:
	.string	"  OK: pixel matches red\n"
	.align 8
.LC41:
	.string	"  [WARN] pixel mismatch (may be expected)\n"
.LC42:
	.string	"  [PASS] framebuffer\n"
	.text
	.globl	test_framebuffer
	.type	test_framebuffer, @function
test_framebuffer:
.LFB26:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$496, %rsp
	leaq	.LC27(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	$0, %edx
	movl	$3, %esi
	leaq	.LC28(%rip), %rax
	movq	%rax, %rdi
	call	sys_open
	movl	%eax, -12(%rbp)
	cmpl	$0, -12(%rbp)
	jns	.L81
	leaq	.LC29(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-12(%rbp), %eax
	cltq
	movq	%rax, %rdi
	call	print_long
	leaq	.LC24(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L80
.L81:
	leaq	.LC30(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-12(%rbp), %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC24(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	-80(%rbp), %rdx
	movl	-12(%rbp), %eax
	movl	$17921, %esi
	movl	%eax, %edi
	call	sys_ioctl
	movq	%rax, -24(%rbp)
	cmpq	$0, -24(%rbp)
	je	.L83
	leaq	.LC31(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	print_long
	leaq	.LC24(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-12(%rbp), %eax
	movl	%eax, %edi
	call	sys_close
	jmp	.L80
.L83:
	leaq	.LC32(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC33(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-72(%rbp), %rax
	movl	%eax, %edi
	call	print_int
	leaq	.LC34(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-64(%rbp), %rax
	movl	%eax, %edi
	call	print_int
	leaq	.LC24(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC35(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-56(%rbp), %rax
	movl	%eax, %edi
	call	print_int
	leaq	.LC36(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movzwl	-48(%rbp), %eax
	movzwl	%ax, %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC24(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC37(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-80(%rbp), %rax
	movq	%rax, %rdi
	call	print_hex
	leaq	.LC24(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movzwl	-48(%rbp), %eax
	cmpw	$32, %ax
	jne	.L84
	movl	$16711680, -28(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L85
.L86:
	movl	-4(%rbp), %eax
	cltq
	movl	-28(%rbp), %edx
	movl	%edx, -496(%rbp,%rax,4)
	addl	$1, -4(%rbp)
.L85:
	cmpl	$99, -4(%rbp)
	jle	.L86
	movl	$0, -8(%rbp)
	jmp	.L87
.L88:
	leaq	-496(%rbp), %rcx
	movl	-12(%rbp), %eax
	movl	$400, %edx
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	sys_write
	addl	$1, -8(%rbp)
.L87:
	cmpl	$99, -8(%rbp)
	jle	.L88
	leaq	.LC38(%rip), %rax
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
	jne	.L84
	leaq	.LC39(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-84(%rbp), %eax
	movl	%eax, %eax
	movq	%rax, %rdi
	call	print_hex
	leaq	.LC24(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-84(%rbp), %eax
	cmpl	%eax, -28(%rbp)
	jne	.L90
	leaq	.LC40(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L84
.L90:
	leaq	.LC41(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L84:
	movl	-12(%rbp), %eax
	movl	%eax, %edi
	call	sys_close
	leaq	.LC42(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L80:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE26:
	.size	test_framebuffer, .-test_framebuffer
	.section	.rodata
	.align 8
.LC43:
	.string	"\n--- TEST: /dev/kbio (keyboard events) ---\n"
.LC44:
	.string	"/dev/kbio"
	.align 8
.LC45:
	.string	"  [FAIL] open /dev/kbio returned "
.LC46:
	.string	"  OK: opened /dev/kbio as fd="
.LC47:
	.string	"  OK: got event ("
.LC48:
	.string	" bytes)\n"
.LC49:
	.string	"  scancode: "
.LC50:
	.string	" pressed: "
	.align 8
.LC51:
	.string	"  [INFO] no events pending (EAGAIN = expected)\n"
.LC52:
	.string	"  [PASS] kbio\n"
	.text
	.globl	test_kbio_events
	.type	test_kbio_events, @function
test_kbio_events:
.LFB27:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	leaq	.LC43(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	$0, %edx
	movl	$1, %esi
	leaq	.LC44(%rip), %rax
	movq	%rax, %rdi
	call	sys_open
	movl	%eax, -4(%rbp)
	cmpl	$0, -4(%rbp)
	jns	.L93
	leaq	.LC45(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-4(%rbp), %eax
	cltq
	movq	%rax, %rdi
	call	print_long
	leaq	.LC24(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L92
.L93:
	leaq	.LC46(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC24(%rip), %rax
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
	jle	.L95
	leaq	.LC47(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	print_long
	leaq	.LC48(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC49(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movzbl	-21(%rbp), %eax
	movzbl	%al, %eax
	movq	%rax, %rdi
	call	print_hex
	leaq	.LC50(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movzbl	-20(%rbp), %eax
	movzbl	%al, %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC24(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L96
.L95:
	leaq	.LC51(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L96:
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	sys_close
	leaq	.LC52(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L92:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE27:
	.size	test_kbio_events, .-test_kbio_events
	.section	.rodata
	.align 8
.LC53:
	.string	"\n--- TEST: /dev/mouse (mouse events) ---\n"
.LC54:
	.string	"/dev/mouse"
	.align 8
.LC55:
	.string	"  [FAIL] open /dev/mouse returned "
	.align 8
.LC56:
	.string	"  OK: opened /dev/mouse as fd="
.LC57:
	.string	"  OK: got state ("
.LC58:
	.string	"  dx="
.LC59:
	.string	" dy="
.LC60:
	.string	" dz="
.LC61:
	.string	" btn=0x"
	.align 8
.LC62:
	.string	"  [INFO] no mouse state change\n"
.LC63:
	.string	"  [PASS] mouse\n"
	.text
	.globl	test_mouse_events
	.type	test_mouse_events, @function
test_mouse_events:
.LFB28:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	leaq	.LC53(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	$0, %edx
	movl	$1, %esi
	leaq	.LC54(%rip), %rax
	movq	%rax, %rdi
	call	sys_open
	movl	%eax, -4(%rbp)
	cmpl	$0, -4(%rbp)
	jns	.L98
	leaq	.LC55(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-4(%rbp), %eax
	cltq
	movq	%rax, %rdi
	call	print_long
	leaq	.LC24(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L97
.L98:
	leaq	.LC56(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC24(%rip), %rax
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
	jle	.L100
	leaq	.LC57(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	print_long
	leaq	.LC48(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC58(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movzbl	-19(%rbp), %eax
	movsbl	%al, %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC59(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movzbl	-18(%rbp), %eax
	movsbl	%al, %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC60(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movzbl	-17(%rbp), %eax
	movsbl	%al, %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC61(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movzbl	-22(%rbp), %eax
	movzbl	%al, %eax
	movq	%rax, %rdi
	call	print_hex
	leaq	.LC24(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L101
.L100:
	leaq	.LC62(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L101:
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	sys_close
	leaq	.LC63(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L97:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE28:
	.size	test_mouse_events, .-test_mouse_events
	.section	.rodata
	.align 8
.LC64:
	.string	"\n--- TEST: anonymous mmap ---\n"
	.align 8
.LC65:
	.string	"  [FAIL] mmap returned MAP_FAILED\n"
.LC66:
	.string	"  OK: got mapping at "
.LC67:
	.string	"  OK: all bytes zeroed\n"
	.align 8
.LC68:
	.string	"  OK: pattern write/read-back passed\n"
.LC69:
	.string	"  [FAIL] munmap failed\n"
.LC70:
	.string	"  OK: munmap succeeded\n"
.LC71:
	.string	"  [PASS] anonymous mmap\n"
	.text
	.globl	test_mmap_anonymous
	.type	test_mmap_anonymous, @function
test_mmap_anonymous:
.LFB29:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	leaq	.LC64(%rip), %rax
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
	jne	.L103
	leaq	.LC65(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L102
.L103:
	leaq	.LC66(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-24(%rbp), %rax
	movq	%rax, %rdi
	call	print_hex
	leaq	.LC24(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-24(%rbp), %rax
	movq	%rax, -32(%rbp)
	movl	$1, -4(%rbp)
	movl	$0, -8(%rbp)
	jmp	.L105
.L108:
	movl	-8(%rbp), %eax
	movslq	%eax, %rdx
	movq	-32(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	je	.L106
	movl	$0, -4(%rbp)
	jmp	.L107
.L106:
	addl	$1, -8(%rbp)
.L105:
	cmpl	$8191, -8(%rbp)
	jle	.L108
.L107:
	cmpl	$0, -4(%rbp)
	je	.L109
	leaq	.LC67(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L109:
	movl	$0, -12(%rbp)
	jmp	.L110
.L111:
	movl	-12(%rbp), %eax
	movslq	%eax, %rdx
	movq	-32(%rbp), %rax
	addq	%rdx, %rax
	movl	-12(%rbp), %edx
	movb	%dl, (%rax)
	addl	$1, -12(%rbp)
.L110:
	cmpl	$8191, -12(%rbp)
	jle	.L111
	movl	$1, -4(%rbp)
	movl	$0, -16(%rbp)
	jmp	.L112
.L115:
	movl	-16(%rbp), %eax
	movslq	%eax, %rdx
	movq	-32(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	movl	-16(%rbp), %edx
	cmpb	%dl, %al
	je	.L113
	movl	$0, -4(%rbp)
	jmp	.L114
.L113:
	addl	$1, -16(%rbp)
.L112:
	cmpl	$8191, -16(%rbp)
	jle	.L115
.L114:
	cmpl	$0, -4(%rbp)
	je	.L116
	leaq	.LC68(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L116:
	movq	-24(%rbp), %rax
	movl	$8192, %esi
	movq	%rax, %rdi
	call	sys_munmap
	testq	%rax, %rax
	je	.L117
	leaq	.LC69(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L102
.L117:
	leaq	.LC70(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC71(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L102:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE29:
	.size	test_mmap_anonymous, .-test_mmap_anonymous
	.section	.rodata
.LC72:
	.string	"\n--- TEST: mprotect ---\n"
.LC73:
	.string	"  [FAIL] mmap failed\n"
.LC74:
	.string	"  [FAIL] mprotect PROT_READ\n"
	.align 8
.LC75:
	.string	"  OK: mprotect PROT_READ applied\n"
.LC76:
	.string	"  [FAIL] not readable\n"
.LC77:
	.string	"  OK: data still readable\n"
.LC78:
	.string	"  [FAIL] restore\n"
.LC79:
	.string	"  [FAIL] write after restore\n"
	.align 8
.LC80:
	.string	"  OK: write after restore succeeded\n"
.LC81:
	.string	"  [PASS] mprotect\n"
	.text
	.globl	test_mmap_mprotect
	.type	test_mmap_mprotect, @function
test_mmap_mprotect:
.LFB30:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	leaq	.LC72(%rip), %rax
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
	jne	.L119
	leaq	.LC73(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L118
.L119:
	movq	-8(%rbp), %rax
	movb	$-66, (%rax)
	movq	-8(%rbp), %rax
	movl	$1, %edx
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_mprotect
	testq	%rax, %rax
	je	.L121
	leaq	.LC74(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_munmap
	jmp	.L118
.L121:
	leaq	.LC75(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	cmpb	$-66, %al
	je	.L122
	leaq	.LC76(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_munmap
	jmp	.L118
.L122:
	leaq	.LC77(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movl	$3, %edx
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_mprotect
	testq	%rax, %rax
	je	.L123
	leaq	.LC78(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_munmap
	jmp	.L118
.L123:
	movq	-8(%rbp), %rax
	movb	$-17, (%rax)
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	cmpb	$-17, %al
	je	.L124
	leaq	.LC79(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_munmap
	jmp	.L118
.L124:
	leaq	.LC80(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_munmap
	leaq	.LC81(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L118:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE30:
	.size	test_mmap_mprotect, .-test_mmap_mprotect
	.section	.rodata
	.align 8
.LC82:
	.string	"\n--- TEST: multiple mappings ---\n"
.LC83:
	.string	"  [FAIL] one mapping failed\n"
.LC84:
	.string	"  OK: 3 mappings created\n"
.LC85:
	.string	"  [FAIL] overlap\n"
.LC86:
	.string	"  [PASS] distinct addresses\n"
	.text
	.globl	test_mmap_multi
	.type	test_mmap_multi, @function
test_mmap_multi:
.LFB31:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	leaq	.LC82(%rip), %rax
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
	je	.L126
	cmpq	$-1, -16(%rbp)
	je	.L126
	cmpq	$-1, -24(%rbp)
	jne	.L127
.L126:
	leaq	.LC83(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	cmpq	$-1, -8(%rbp)
	je	.L128
	movq	-8(%rbp), %rax
	movl	$4096, %esi
	movq	%rax, %rdi
	call	sys_munmap
.L128:
	cmpq	$-1, -16(%rbp)
	je	.L129
	movq	-16(%rbp), %rax
	movl	$8192, %esi
	movq	%rax, %rdi
	call	sys_munmap
.L129:
	cmpq	$-1, -24(%rbp)
	je	.L135
	movq	-24(%rbp), %rax
	movl	$16384, %esi
	movq	%rax, %rdi
	call	sys_munmap
	jmp	.L135
.L127:
	leaq	.LC84(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-8(%rbp), %rax
	cmpq	-16(%rbp), %rax
	je	.L132
	movq	-16(%rbp), %rax
	cmpq	-24(%rbp), %rax
	je	.L132
	movq	-8(%rbp), %rax
	cmpq	-24(%rbp), %rax
	jne	.L133
.L132:
	leaq	.LC85(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L134
.L133:
	leaq	.LC86(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L134:
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
	jmp	.L125
.L135:
	nop
.L125:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE31:
	.size	test_mmap_multi, .-test_mmap_multi
	.section	.rodata
.LC87:
	.string	"\n=== Unix Syscall Tests ===\n"
.LC88:
	.string	"argv["
.LC89:
	.string	"] = "
.LC90:
	.string	"\n=== All Tests Complete ===\n"
	.text
	.globl	main_program
	.type	main_program, @function
main_program:
.LFB32:
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
	leaq	.LC87(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	$0, -4(%rbp)
	jmp	.L137
.L138:
	leaq	.LC88(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC89(%rip), %rax
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
	leaq	.LC24(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	addl	$1, -4(%rbp)
.L137:
	movl	-4(%rbp), %eax
	cmpl	-20(%rbp), %eax
	jl	.L138
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
	leaq	.LC90(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE32:
	.size	main_program, .-main_program
	.section	.rodata
.LC91:
	.string	"\nArguments:\n"
.LC92:
	.string	"  argc = "
.LC93:
	.string	"\nEnvironment:\n"
.LC94:
	.string	"  envp["
.LC95:
	.string	"\nDone.\n"
	.text
	.globl	_start
	.type	_start, @function
_start:
.LFB33:
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
	leaq	.LC91(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC92(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-52(%rbp), %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC24(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC93(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	cmpq	$0, -72(%rbp)
	je	.L140
	movq	-72(%rbp), %rax
	movq	(%rax), %rax
	testq	%rax, %rax
	jne	.L141
.L140:
	leaq	.LC20(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L142
.L141:
	movl	$0, -4(%rbp)
	jmp	.L143
.L144:
	leaq	.LC94(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC89(%rip), %rax
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
	leaq	.LC24(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	addl	$1, -4(%rbp)
.L143:
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	movq	-72(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	testq	%rax, %rax
	jne	.L144
.L142:
	movq	-64(%rbp), %rdx
	movl	-52(%rbp), %eax
	movq	%rdx, %rsi
	movl	%eax, %edi
	call	main_program
	call	sys_fuck_you
	leaq	.LC95(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE33:
	.size	_start, .-_start
	.section	.rodata
	.align 16
	.type	hex.0, @object
	.size	hex.0, 17
hex.0:
	.string	"0123456789abcdef"
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
