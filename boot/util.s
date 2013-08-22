.code16
.section	.btext, "ax"

# print_msg(s string)
.type print_msg_16, @function
.globl print_msg_16
print_msg_16:
	pushw %bp
	movw %sp, %bp
	cld
	movw 4(%bp), %si # *string
1:
	lodsb
	andb %al, %al
	jz 2f
	movb $0xe, %ah
	movw $7, %bx
	int $0x10
	jmp 1b
2:
	# sp must not move
	popw %bp
	ret

# read_sect(num int)
.type read_sect, @function
.globl read_sect
read_sect:
	pushw %bp
	movw %sp, %bp
	movw $0x1000, %ax
	movw %ax, %es
	movb $2, %ah
	movb 4(%bp), %al
	xorw %bx, %bx
	xorb %ch, %ch
	movb $2, %cl
	xorb %dh, %dh
	movb $0x80, %dl
	int $0x13
	leave
	ret

.code32
.section .text
# tw_memset(char *s, char v, int l)
.type tw_memset, @function
.globl tw_memset
tw_memset:
	pushl %ebp
	movl %esp, %ebp

	pushl %eax
	pushl %edi
	pushl %ecx
	
	movl 8(%ebp), %edi
	movb 12(%ebp), %al
	movl 16(%ebp), %ecx
	rep stosb

	popl %ecx
	popl %ebx
	popl %eax

	leave
	ret
