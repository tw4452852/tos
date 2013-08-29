.section	.bdata, "aw"
#include "common.h"

gdt: .quad GDT_DESC(0, 0, 0)
desc_code32: .quad GDT_DESC(0, 0xffffffff, GDT_CODE_PL0)
desc_data32: .quad GDT_DESC(0, 0xffffffff, GDT_DATA_PL0)
.set gdt_len, .-gdt
.set sel_code32, desc_code32 - gdt
.set sel_data32, desc_data32 - gdt
gdt_ptr:
       .word gdt_len - 1
gdt_addr:
       .long 0
boot_msg: .string "hello bootsec"
.section	.btext, "ax"
.code16
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
	xorl %eax, %eax
	movw %ds, %ax
	sal $4, %eax
	addl $gdt, %eax
	movl %eax, gdt_addr
	lgdt gdt_ptr

	# enable protect mode
	movl %cr0, %eax
	orl $1, %eax
	movl %eax, %cr0

	ljmp $sel_code32, $_start_32

.code32
_start_32:
	movw $sel_data32, %ax
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss
	movl $STACK_TOP, %esp # 16M stack

	call cmain

	# shouldn't reach here
	jmp .

.code16
# some helpers in real mode
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
