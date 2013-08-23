.code16
.section	.bdata, "aw"
.include "def.h"
gdt: descriptor 0, 0, 0
desc_code32: descriptor 0, 0xffffffff, GDT_CODE_PL0
desc_data32: descriptor 0, 0xffffffff, GDT_DATA_PL0
.set gdt_len, .-gdt

.set sel_code32, desc_code32 - gdt
.set sel_data32, desc_data32 - gdt
gdt_ptr:
	.word gdt_len - 1
gdt_addr:
	.long 0
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
	call read_sect
	addl $2, %esp

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
	movl $0X1000000, %esp # 256byte stack

	call cmain

	jmp .
