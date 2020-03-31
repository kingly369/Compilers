main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$main.size, %esp


	movl	$0, %eax
	movl	%eax, -4(%ebp)
.L3:


	movl	-4(%ebp), %eax
	cmpl	$10, %eax
	setl	%al
	movzbl	%al, %eax
	movl	%eax, -16(%ebp)
	movl	-16(%ebp), %eax
	cmpl	$0, %eax
	je	.L4


	movl	-4(%ebp), %eax
	addl	$1, %eax
	movl	%eax, -20(%ebp)
	movl	-20(%ebp), %eax
	movl	%eax, -8(%ebp)
.L5:


	movl	-4(%ebp), %eax
	cmpl	-8(%ebp), %eax
	setl	%al
	movzbl	%al, %eax
	movl	%eax, -24(%ebp)
	movl	-24(%ebp), %eax
	cmpl	$0, %eax
	je	.L6


	leal	.L7, %eax
	movl	%eax, -28(%ebp)
	movl	-28(%ebp), %eax
	movl	%eax, 0(%esp)
	movl	-8(%ebp), %eax
	movl	%eax, 4(%esp)
	call	printf
	movl	%eax, -32(%ebp)


	movl	$6, %eax
	movl	%eax, -12(%ebp)
.L8:


	movl	-12(%ebp), %eax
	cmpl	$0, %eax
	setg	%al
	movzbl	%al, %eax
	movl	%eax, -36(%ebp)
	movl	-36(%ebp), %eax
	cmpl	$0, %eax
	je	.L9



	movl	-12(%ebp), %eax
	cmpl	-4(%ebp), %eax
	sete	%al
	movzbl	%al, %eax
	movl	%eax, -40(%ebp)
	movl	-40(%ebp), %eax
	cmpl	$0, %eax
	je	.L10
	jmp	.L9
	jmp	.L11
.L10:
.L11:


	leal	.L12, %eax
	movl	%eax, -44(%ebp)
	movl	-44(%ebp), %eax
	movl	%eax, 0(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	call	printf
	movl	%eax, -48(%ebp)

	movl	-12(%ebp), %eax
	movl	%eax, -52(%ebp)
	subl	$1, %eax
	movl	%eax, -12(%ebp)
	jmp	.L8
.L9:

	movl	-8(%ebp), %eax
	movl	%eax, -56(%ebp)
	subl	$1, %eax
	movl	%eax, -8(%ebp)



	movl	-8(%ebp), %eax
	cmpl	$5, %eax
	sete	%al
	movzbl	%al, %eax
	movl	%eax, -60(%ebp)
	movl	-60(%ebp), %eax
	cmpl	$0, %eax
	je	.L13
	jmp	.L9
	jmp	.L14
.L13:
.L14:
	jmp	.L5
.L6:

	movl	-4(%ebp), %eax
	movl	%eax, -64(%ebp)
	addl	$1, %eax
	movl	%eax, -4(%ebp)
	jmp	.L3
.L4:
.L1:
	movl	%ebp, %esp
	popl	%ebp
	ret

	.set	main.size, 72
	.globl	main

.data
.L12:	.asciz	"Printing k: %d"
.L7:	.asciz	"Printing j: %d"
