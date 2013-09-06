BINFILE := boot.bin
ASMFILES := $(wildcard *.S)
CFILES := $(wildcard *.c)
HDRFILES := $(wildcard *.h)
OBJFILES := $(ASMFILES:.S=.o)
OBJFILES += $(CFILES:.c=.o)
DEPFILES := $(OBJFILES:.o=.d)
LDSCRIPT := boot.ld
LDFLAGS := -T $(LDSCRIPT)
CFLAGS := -m32 -MMD -Wall
ASFLAGS := $(CFLAGS) -D__ASSEMBLY__
HDIMAGE := hd.img

.PHONY: all clean

all: $(BINFILE) $(HDIMAGE)
	dd if=$(BINFILE) of=$(HDIMAGE) bs=1M count=32 conv=notrunc

$(HDIMAGE):
	dd if=/dev/zero of=$@ bs=1M count=32

$(BINFILE): $(OBJFILES)
	ld $(LDFLAGS) -o $@ $+

clean:
	-@rm -fr $(OBJFILES) $(DEPFILES) $(BINFILE)

-include $(DEPFILES)
