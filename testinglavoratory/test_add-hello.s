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
# 6 "hello.c" 1
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
.LFB1:
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
	call	syscall
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	sys_print, .-sys_print
	.type	print_str, @function
print_str:
.LFB2:
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
	jmp	.L6
.L7:
	addq	$1, -8(%rbp)
.L6:
	movq	-8(%rbp), %rax
	movzbl	(%rax), %eax
	testb	%al, %al
	jne	.L7
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
.LFE2:
	.size	print_str, .-print_str
	.section	.rodata
.LC0:
	.string	"argv["
.LC1:
	.string	"] = "
.LC2:
	.string	"\n"
	.text
	.globl	main_program
	.type	main_program, @function
main_program:
.LFB3:
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
	jmp	.L9
.L10:
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-4(%rbp), %eax
	addl	$48, %eax
	movb	%al, -5(%rbp)
	leaq	-5(%rbp), %rax
	movl	$1, %esi
	movq	%rax, %rdi
	call	sys_print
	leaq	.LC1(%rip), %rax
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
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	addl	$1, -4(%rbp)
.L9:
	movl	-4(%rbp), %eax
	cmpl	-20(%rbp), %eax
	jl	.L10
	nop
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	main_program, .-main_program
	.section	.rodata
	.align 8
.LC3:
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
	.ascii	". Phasellus ul"
	.string	"trices finibus nisi, in rutrum ante eleifend ac. Duis mi sapien, rhoncus ac enim id, molestie imperdiet sem. Ut id tortor in ligula viverra dignissim. Donec purus risus, blandit sed est nec, ullamcorper vestibulum ipsum. Etiam pharetra feugiat facilisis. \n"
.LC4:
	.string	"Environment variables:\n"
.LC5:
	.string	"  <none>\n"
.LC6:
	.string	"envp["
	.text
	.globl	_start
	.type	_start, @function
_start:
.LFB4:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$112, %rsp
	movl	%edi, -84(%rbp)
	movq	%rsi, -96(%rbp)
	movq	%rdx, -104(%rbp)
	movabsq	$4908972450905875784, %rax
	movabsq	$2315223284149546863, %rdx
	movq	%rax, -64(%rbp)
	movq	%rdx, -56(%rbp)
	movabsq	$2327603797202848072, %rax
	movabsq	$729940808559894595, %rdx
	movq	%rax, -48(%rbp)
	movq	%rdx, -40(%rbp)
	movb	$0, -32(%rbp)
	leaq	.LC3(%rip), %rax
	movq	%rax, -16(%rbp)
	leaq	-64(%rbp), %rax
	movl	$32, %esi
	movq	%rax, %rdi
	call	sys_print
	movq	-16(%rbp), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-96(%rbp), %rdx
	movl	-84(%rbp), %eax
	movq	%rdx, %rsi
	movl	%eax, %edi
	call	main_program
	leaq	.LC4(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movq	-104(%rbp), %rax
	movq	(%rax), %rax
	testq	%rax, %rax
	jne	.L12
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	jmp	.L16
.L12:
	movl	$0, -4(%rbp)
	jmp	.L14
.L15:
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-4(%rbp), %eax
	addl	$48, %eax
	movb	%al, -65(%rbp)
	leaq	-65(%rbp), %rax
	movl	$1, %esi
	movq	%rax, %rdi
	call	sys_print
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	movq	-104(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	print_str
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	call	print_str
	addl	$1, -4(%rbp)
.L14:
	movl	-4(%rbp), %eax
	cltq
	leaq	0(,%rax,8), %rdx
	movq	-104(%rbp), %rax
	addq	%rdx, %rax
	movq	(%rax), %rax
	testq	%rax, %rax
	jne	.L15
.L16:
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
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
