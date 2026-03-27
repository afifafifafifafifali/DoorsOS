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
# 13 "hello.c" 1
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
	.type	syscall3, @function
syscall3:
.LFB1:
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
.LFE1:
	.size	syscall3, .-syscall3
	.type	sys_print, @function
sys_print:
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
	movq	-8(%rbp), %rax
	movq	-16(%rbp), %rdx
	movq	%rdx, %rcx
	movq	%rax, %rdx
	movl	$1, %esi
	movl	$67671, %edi
	call	syscall3
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	sys_print, .-sys_print
	.type	syscall1, @function
syscall1:
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
.LFE3:
	.size	syscall1, .-syscall1
	.type	sys_uname, @function
sys_uname:
.LFB4:
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
.LFE4:
	.size	sys_uname, .-sys_uname
	.type	sys_fuck_you, @function
sys_fuck_you:
.LFB5:
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
.LFE5:
	.size	sys_fuck_you, .-sys_fuck_you
	.type	print_str, @function
print_str:
.LFB6:
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
	jmp	.L14
.L15:
	addq	$1, -8(%rbp)
.L14:
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	jne	.L15
	movq	-8(%rbp), %rax
	subq	-24(%rbp), %rax
	movq	%rax, %rdx
	movq	-24(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	sys_print
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	print_str, .-print_str
	.section	.rodata
.LC0:
	.string	"0"
	.text
	.type	print_int, @function
print_int:
.LFB7:
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
	jne	.L17
	movl	$1, %esi
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	call	sys_print
	jmp	.L16
.L17:
	movl	$0, -4(%rbp)
	movl	$0, -8(%rbp)
	cmpl	$0, -52(%rbp)
	jns	.L20
	movl	$1, -8(%rbp)
	negl	-52(%rbp)
	jmp	.L20
.L21:
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
.L20:
	cmpl	$0, -52(%rbp)
	jg	.L21
	cmpl	$0, -8(%rbp)
	je	.L22
	movl	-4(%rbp), %eax
	leal	1(%rax), %edx
	movl	%edx, -4(%rbp)
	cltq
	movb	$45, -48(%rbp,%rax)
.L22:
	movl	$0, -12(%rbp)
	jmp	.L23
.L24:
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
.L23:
	movl	-4(%rbp), %eax
	movl	%eax, %edx
	shrl	$31, %edx
	addl	%edx, %eax
	sarl	%eax
	cmpl	%eax, -12(%rbp)
	jl	.L24
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	leaq	-48(%rbp), %rax
	movq	%rdx, %rsi
	movq	%rax, %rdi
	call	sys_print
.L16:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	print_int, .-print_int
	.section	.rodata
.LC1:
	.string	"argv["
.LC2:
	.string	"] = "
.LC3:
	.string	"\n"
	.text
	.globl	main_program
	.type	main_program, @function
main_program:
.LFB8:
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
	movl	$0, -4(%rbp)
	jmp	.L26
.L27:
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC2(%rip), %rax
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
	leaq	.LC3(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	addl	$1, -4(%rbp)
.L26:
	movl	-4(%rbp), %eax
	cmpl	-20(%rbp), %eax
	jl	.L27
	nop
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	main_program, .-main_program
	.section	.rodata
.LC4:
	.string	"sysname: "
.LC5:
	.string	"nodename: "
.LC6:
	.string	"release: "
.LC7:
	.string	"version: "
.LC8:
	.string	"machine: "
.LC9:
	.string	"sys_uname failed\n"
	.text
	.globl	print_uname
	.type	print_uname, @function
print_uname:
.LFB9:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$336, %rsp
	leaq	-336(%rbp), %rax
	movq	%rax, %rdi
	call	sys_uname
	testq	%rax, %rax
	jne	.L29
	leaq	.LC4(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	-336(%rbp), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC3(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	-336(%rbp), %rax
	addq	$65, %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC3(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	-336(%rbp), %rax
	addq	$130, %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC3(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC7(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	-336(%rbp), %rax
	addq	$195, %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC3(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC8(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	-336(%rbp), %rax
	addq	$260, %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC3(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L31
.L29:
	leaq	.LC9(%rip), %rax
	movq	%rax, %rdi
	call	print_str
.L31:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE9:
	.size	print_uname, .-print_uname
	.section	.rodata
	.align 8
.LC10:
	.ascii	"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam"
	.ascii	" hendrerit nulla eget imperdiet varius. Cras at accumsan orc"
	.ascii	"i, non sodales eros. Aenean tincidunt tellus justo, eu vulpu"
	.ascii	"tate dui eleifend sit amet. Sed eu nunc volutpat, scelerisqu"
	.ascii	"e libero in, euismod enim. Orci varius natoque penatibus et "
	.ascii	"magnis dis parturient montes, nascetur ridiculus mus. Maecen"
	.ascii	"as efficitur accumsan enim, in tempus justo dignissim ac. Do"
	.ascii	"nec aliquam dignissim volutpat. Praesent mattis dui ac odio "
	.ascii	"mattis luctus. Sed condimentum consectetur tempus. Sed vesti"
	.ascii	"bulum erat eget pellentesque pharetra. Proin et luctus metus"
	.ascii	".\n\nDonec malesuada ipsum tellus, eu consequat odio ullamco"
	.ascii	"rper non. Proin cursus nec dolor vel porta. Aenean ac velit "
	.ascii	"nisi. Proin nibh libero, tincidunt nec blandit nec, porttito"
	.ascii	"r quis massa. Praesent pellentesque lectus eu orci malesuada"
	.ascii	", quis ultrices tellus malesuada. Curabitur eu tristique dia"
	.ascii	"m. Proin finibus nisi ligula, ut posuere diam elementum a.\n"
	.ascii	"\nPraesent luctus venenatis dui eget pulvinar. Praesent just"
	.ascii	"o urna, convallis eu fermentum vitae, lobortis id quam. Aliq"
	.ascii	"uam non dolor finibus, laoreet mauris consectetur, maximus n"
	.ascii	"eque. Aliquam sit amet commodo enim. Sed tempor pulvinar fel"
	.ascii	"is, sit amet faucibus neque fermentum sed. Nulla et euismod "
	.ascii	"nunc, at bibendum odio. Nam neque justo, sagittis ut nulla h"
	.ascii	"endrerit, semper varius nisi. Donec nec mattis neque. Cras u"
	.ascii	"ltrices ipsum sed lectus sollicitudin pellentesque. Duis max"
	.ascii	"imus ligula magna. Sed aliquet dictum mi.\n\nVestibulum sit "
	.ascii	"amet dolor eu turpis venenatis vulputate. Vestibulum sed ali"
	.ascii	"quet libero. Fusce vestibulum nisi turpis, ac molestie est s"
	.ascii	"agittis non. Nunc eu enim odio. Maecenas id felis neque. Cra"
	.ascii	"s luctus metus vel orci tempor tempor. Sed nec erat lacus. S"
	.ascii	"ed ultricies varius elit ac blandit. Vestibulum ut rutrum lo"
	.ascii	"rem. Fusce varius, dolor at malesuada pretium, nulla nisi te"
	.ascii	"mpus dolor, in vestibulum sem ante efficitur erat. Phasellus"
	.ascii	" sed velit id justo egestas porta ut et massa.\n\nAenean pur"
	.ascii	"us felis, semper a dapibus i"
	.ascii	"d, posuere ac est. Morbi in pulvinar ligula. Sed ullamcorper"
	.ascii	" sapien nec nulla sollicitudin sollicitudin. In viverra enim"
	.ascii	" quis turpis facilisis, non mollis metus mollis. Donec sed e"
	.ascii	"leifend mi. Duis ultricies odio ex, ultrices mattis ipsum te"
	.ascii	"mpor et. Morbi rhoncus nulla sit amet arcu pulvinar bibendum"
	.ascii	". Integer sed tellus faucibus, feugiat magna ac, luctus arcu"
	.ascii	". Phasellus u"
	.string	"ltrices finibus nisi, in rutrum ante eleifend ac. Duis mi sapien, rhoncus ac enim id, molestie imperdiet sem. Ut id tortor in ligula viverra dignissim. Donec purus risus, blandit sed est nec, ullamcorper vestibulum ipsum. Etiam pharetra feugiat facilisis.\n"
.LC11:
	.string	"Environment variables:\n"
.LC12:
	.string	"  <none>\n"
.LC13:
	.string	"envp["
	.text
	.globl	_start
	.type	_start, @function
_start:
.LFB10:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$96, %rsp
	movl	%edi, -68(%rbp)
	movq	%rsi, -80(%rbp)
	movq	%rdx, -88(%rbp)
	movabsq	$4908972450905875784, %rax
	movabsq	$2315223284149546863, %rdx
	movq	%rax, -64(%rbp)
	movq	%rdx, -56(%rbp)
	movabsq	$2327603797202848072, %rax
	movabsq	$729940808559894595, %rdx
	movq	%rax, -48(%rbp)
	movq	%rdx, -40(%rbp)
	movb	$0, -32(%rbp)
	leaq	.LC10(%rip), %rax
	movq	%rax, -16(%rbp)
	movl	$0, %eax
	call	print_uname
	leaq	-64(%rbp), %rax
	movl	$32, %esi
	movq	%rax, %rdi
	call	sys_print
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-80(%rbp), %rdx
	movl	-68(%rbp), %eax
	movq	%rdx, %rsi
	movl	%eax, %edi
	call	main_program
	leaq	.LC11(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	cmpq	$0, -88(%rbp)
	je	.L33
	movq	-88(%rbp), %rax
	movq	(%rax), %rax
	testq	%rax, %rax
	jne	.L34
.L33:
	leaq	.LC12(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L35
.L34:
	movl	$0, -4(%rbp)
	jmp	.L36
.L37:
	leaq	.LC13(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-4(%rbp), %eax
	movl	%eax, %edi
	call	print_int
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	movq	-88(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC3(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	addl	$1, -4(%rbp)
.L36:
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	movq	-88(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	testq	%rax, %rax
	jne	.L37
.L35:
	movl	$0, %eax
	call	sys_fuck_you
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE10:
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
