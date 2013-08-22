.macro descriptor base, limit, attr
	.word \limit & 0xffff
	.word \base & 0xffff
	.byte (\base>>16) & 0xff
	.byte \attr & 0xff
	.byte (((\attr>>8)<<4) & 0xff00) | ((\limit>>16) & 0xff)
	.byte (\base>>24) & 0xff
.endm

.set GDT_CODE_PL0, DA_G | DA_P | DA_SEG32 | DA_TYPE | DA_CODE_EXRD
.set GDT_DATA_PL0, DA_G | DA_P | DA_SEG32 | DA_TYPE | DA_DATA_RDWR
.set GDT_CODE_PL3, DA_G | DA_P | DA_SEG32 | DA_TYPE | DA_CODE_EXRD | DA_DPL3
.set GDT_CODE_PL3, DA_G | DA_P | DA_SEG32 | DA_TYPE | DA_DATA_RDWR | DA_DPL3
.set DA_G, 1<<11
.set DA_SEG32, 1<<10
.set DA_P, 1<<7
.set DA_DPL0, 0
.set DA_DPL1, 1<<5
.set DA_DPL2, 2<<5
.set DA_DPL3, 3<<5
.set DA_TYPE, 1<<4
.set DA_DATA_RD, 0x00
.set DA_DATA_RDA, 0x01
.set DA_DATA_RDWR, 0x02
.set DA_DATA_RDWRA, 0x03
.set DA_DATA_RDEXPD, 0x04
.set DA_DATA_RDEXPDA, 0x05
.set DA_DATA_RDWREXPD, 0x06
.set DA_DATA_RDWREXPDA, 0x07
.set DA_CODE_EX, 0x08
.set DA_CODE_EXA, 0x09
.set DA_CODE_EXRD, 0x0a
.set DA_CODE_EXRDA, 0x0b
.set DA_CODE_EXC, 0x0c
.set DA_CODE_EXCA, 0X0d
.set DA_CODE_EXRDC, 0x0e
.set DA_CODE_EXRDCA, 0x0f
