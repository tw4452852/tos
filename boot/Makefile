TARGET=boot.bin
OBJS=boot.o main.o video.o mem.o interrupt.o gdt.o
LDSCRIPT=boot.ld
LDFLAGS=-T $(LDSCRIPT)
CFLAGS=-m32
ASFLAGS=-m32 -D__ASSEMBLY__
HDIMAGE=hd.img

.PHONY: all
all: clean $(TARGET) $(HDIMAGE)
	dd if=$(TARGET) of=$(HDIMAGE) bs=1M count=32 conv=notrunc

$(HDIMAGE):
	dd if=/dev/zero of=$@ bs=1M count=32

$(TARGET): $(OBJS)
	ld $(LDFLAGS) -o $@ $+

.PHONY: clean
clean:
	rm -fr $(OBJS) $(TARGET)
