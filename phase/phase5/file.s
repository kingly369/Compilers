.comm a, 40
.globl main

main:
	pushl %ebp
	movl %esp, %ebp
	subl $main.size, %esp
	movl $5, -88(%ebp)
	movl $2, -92(%ebp)
	movl $4, -96(%ebp)
	movl $1, -92(%ebp)
	movl $10, -4(%ebp)
	movl -4(%ebp), %eax
	movl %eax, 0(%esp)
	call init_array
	movl -4(%ebp), %eax
	movl %eax, 0(%esp)
	call print_array
	movl %ebp, %esp
	popl %ebp
	ret
.set main.size, 104
