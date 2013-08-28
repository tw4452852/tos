.code16
.section	.bdata, "aw"
#include "def.h"
boot_msg: .string "hello bootsec"
.section	.btext, "ax"
.globl _start
_start:
	movw %cs, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %ss
	movw $0x100, %sp

	pushw $boot_msg
	call print_msg_16
	addw $2, %sp

	call load_pmem_map

	pushw $100
	pushw $KERNEL_ADDR_OFFSET
	pushw $KERNEL_ADDR_SEG
	call read_sect
	addw $6, %sp

	# setup gdt
#	xorl %eax, %eax
#	movw %ds, %ax
#	sal $4, %eax
#	addl $gdt, %eax
#	movl %eax, gdt_addr
#	lgdt gdt_ptr
#
#	# enable protect mode
#	movl %cr0, %eax
#	orl $1, %eax
#	movl %eax, %cr0
#
#	ljmp $sel_code32, $_start_32
#
#.code32
#_start_32:
#	movw $sel_data32, %ax
#	movw %ax, %ds
#	movw %ax, %es
#	movw %ax, %fs
#	movw %ax, %gs
#	movw %ax, %ss
#	movl $0X1000000, %esp # 256byte stack

	ljmp $KERNEL_ADDR_SEG, $KERNEL_ADDR_OFFSET

	# shouldn't reach here
	jmp .

# print_msg(char *s)
.type print_msg_16, @function
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
	leave
	ret

# read_sect(u16 seg, u16 offset, u16 num_sec)
.type read_sect, @function
read_sect:
	pushw %bp
	movw %sp, %bp
	movw 4(%bp), %ax
	movw %ax, %es
	movb $2, %ah
	movb 8(%bp), %al
	movw 6(%bp), %bx
	xorb %ch, %ch
	movb $2, %cl
	xorb %dh, %dh
	movb $0x80, %dl
	int $0x13
	leave
	ret
