TARGET=boot.bin
OBJS=boot.o util.o main.o video.o mem.o
LDSCRIPT=boot.ld
LDFLAGS=-T $(LDSCRIPT)
ASFLAGS=--32
CFLAGS=-m32
HDIMAGE=hd.img

.PHONY: all
all: clean $(TARGET) $(HDIMAGE)
	dd if=$(TARGET) of=$(HDIMAGE) bs=1M count=32 conv=notrunc

$(HDIMAGE):
	dd if=/dev/zero of=$@ bs=1M count=32

$(TARGET): $(OBJS)
	ld $(LDFLAGS) -o $@ $+

%.o: %.s
	as $(ASFLAGS) -o $@ $+

.PHONY: clean
clean:
	rm -fr $(OBJS) $(TARGET)
