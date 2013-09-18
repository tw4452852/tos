OUTPUT_FORMAT("binary")
OUTPUT_ARCH("i386")
ENTRY(_start)
MEMORY
{
	BOOT : ORIGIN = 0x7c00, LENGTH = 512
	MAIN : ORIGIN = 0X10000, LENGTH = 16M
	PAGE : ORIGIN = 0X100000, LENGTH = 8M
}
SECTIONS
{
	.boot :
	{
		_boot_start = .;
		*(.btext);
		*(.bdata);
		. = _boot_start + 510;
		SHORT(0XAA55);
	} > BOOT = 0

	.main :
	AT(ADDR(.boot) + SIZEOF(.boot))
	{
		*(.text)
		*(.*)
	}  > MAIN = 0

	.page :
	{
		_cmain_end = .;
	} > PAGE = 0

}
