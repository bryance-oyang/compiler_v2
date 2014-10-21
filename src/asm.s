.global main
main:
	movq	$3, %rax
	pushq	%rax
	movq	$5, %rax
	pushq	%rax
	movq	$7, %rax
	pushq	%rax
	xorq	%rax, %rax
	popq	%rbx
	addq	%rbx, %rax
	popq	%rbx
	addq	%rbx, %rax
	popq	%rbx
	addq	%rbx, %rax
	movq	$1, %rax
	pushq	%rax
	movq	$1, %rax
	pushq	%rax
	xorq	%rax, %rax
	popq	%rbx
	addq	%rbx, %rax
	popq	%rbx
	addq	%rbx, %rax
	ret
