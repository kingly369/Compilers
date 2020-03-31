main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$main.size, %esp

	movl	$100, %eax
	movl	%eax, -16(%ebp)

	movl	$30, %eax
	movl	%eax, -20(%ebp)


	fldl	.L3
	fstpl	-44(%ebp)


	movl	$1, %eax
	movl	%eax, -46(%ebp)
	movb	-46(%ebp), %al
	movb	%al, -45(%ebp)






	movl	-16(%ebp), %eax
	addl	-20(%ebp), %eax
	movl	%eax, -50(%ebp)
	fildl	-50(%ebp)
	fstpl	-58(%ebp)
	fldl	-58(%ebp)
	faddl	-44(%ebp)
	fstpl	-66(%ebp)

	fild	-45(%ebp)
	fstpl	-74(%ebp)
	fldl	-66(%ebp)
	faddl	-74(%ebp)
	fstpl	-82(%ebp)
	fldl	-82(%ebp)
	fisttpl	-86(%ebp)
	movl	-86(%ebp), %eax
	movl	%eax, -4(%ebp)





	movl	-16(%ebp), %eax
	subl	-20(%ebp), %eax
	movl	%eax, -90(%ebp)
	fildl	-90(%ebp)
	fstpl	-98(%ebp)
	fldl	-98(%ebp)
	fsubl	-44(%ebp)
	fstpl	-106(%ebp)
	fldl	-106(%ebp)
	fisttpl	-110(%ebp)
	movl	-110(%ebp), %eax
	movl	%eax, -8(%ebp)





	movl	-16(%ebp), %eax
	imull	-20(%ebp), %eax
	movl	%eax, -114(%ebp)
	fildl	-114(%ebp)
	fstpl	-122(%ebp)
	fldl	-122(%ebp)
	fmull	-44(%ebp)
	fstpl	-130(%ebp)
	fldl	-130(%ebp)
	fisttpl	-134(%ebp)
	movl	-134(%ebp), %eax
	movl	%eax, -12(%ebp)




	movl	-16(%ebp), %eax
	cltd
	movl	-20(%ebp), %ecx
	idivl	%ecx
	movl	%eax, -138(%ebp)
	fildl	-138(%ebp)
	fstpl	-146(%ebp)
	fldl	-146(%ebp)
	faddl	-44(%ebp)
	fstpl	-154(%ebp)
	fldl	-154(%ebp)
	fstpl	-28(%ebp)




	movl	-16(%ebp), %eax
	movl	%eax, %edx
	movl	-20(%ebp), %ecx
	sarl	$31, %edx
	idivl	%ecx
	movl	%edx, -158(%ebp)
	fildl	-158(%ebp)
	fstpl	-166(%ebp)
	fldl	-166(%ebp)
	fsubl	-44(%ebp)
	fstpl	-174(%ebp)
	fldl	-174(%ebp)
	fstpl	-36(%ebp)


	leal	.L4, %eax
	movl	%eax, -178(%ebp)
	movl	-178(%ebp), %eax
	movl	%eax, 0(%esp)
	movl	-4(%ebp), %eax
	movl	%eax, 4(%esp)
	call	printf
	movl	%eax, -182(%ebp)


	leal	.L5, %eax
	movl	%eax, -186(%ebp)
	movl	-186(%ebp), %eax
	movl	%eax, 0(%esp)
	movl	-8(%ebp), %eax
	movl	%eax, 4(%esp)
	call	printf
	movl	%eax, -190(%ebp)


	leal	.L6, %eax
	movl	%eax, -194(%ebp)
	movl	-194(%ebp), %eax
	movl	%eax, 0(%esp)
	movl	-12(%ebp), %eax
	movl	%eax, 4(%esp)
	call	printf
	movl	%eax, -198(%ebp)


	leal	.L7, %eax
	movl	%eax, -202(%ebp)
	movl	-202(%ebp), %eax
	movl	%eax, 0(%esp)
	fldl	-28(%ebp)
	fstpl	4(%esp)
	call	printf
	movl	%eax, -206(%ebp)


	leal	.L8, %eax
	movl	%eax, -210(%ebp)
	movl	-210(%ebp), %eax
	movl	%eax, 0(%esp)
	fldl	-36(%ebp)
	fstpl	4(%esp)
	call	printf
	movl	%eax, -214(%ebp)
.L1:
	movl	%ebp, %esp
	popl	%ebp
	ret

	.set	main.size, 232
	.globl	main

.data
.L8:	.asciz	"%f\012"
.L7:	.asciz	"%f\012"
.L6:	.asciz	"%d\012"
.L5:	.asciz	"%d\012"
.L4:	.asciz	"%d\012"
.L3:	.double	2.5
