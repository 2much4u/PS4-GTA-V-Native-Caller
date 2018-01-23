TEXT	:=	0x926200000
DATA	:=	0x926300000

CC		 := gcc
AS		 := gcc
OBJCOPY	 :=	objcopy
ODIR	 :=	build
SDIR	 :=	source
IDIRS	 :=	-Iinclude
LDIRS	 :=	-Llib
CFLAGS	 :=	$(IDIRS) $(LDIRS) -O3 -s -w -std=gnu++11 -fno-builtin -nostartfiles -nostdlib -masm=intel -march=btver2 -mtune=btver2 -m64 -mabi=sysv -mcmodel=large -fpermissive -DTEXT_ADDRESS=$(TEXT) -DDATA_ADDRESS=$(DATA)
LFLAGS	 :=	-Xlinker -T linker.x -Wl,--build-id=none -Ttext=$(TEXT) -Tdata=$(DATA)
CFILES	 :=	$(wildcard $(SDIR)/*.cpp)
SFILES	 :=	$(wildcard $(SDIR)/*.s)
OBJS	 :=	$(patsubst $(SDIR)/%.cpp, $(ODIR)/%.o, $(CFILES)) $(patsubst $(SDIR)/%.s, $(ODIR)/%.o, $(SFILES))

TARGET = $(shell basename $(CURDIR)).bin

$(TARGET): $(ODIR) $(OBJS)
	$(CC) crt0.s $(ODIR)/*.o -o temp.t $(CFLAGS) $(LFLAGS)
	$(OBJCOPY) -O binary temp.t $(TARGET)
	rm -f temp.t

$(ODIR)/%.o: $(SDIR)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: $(SDIR)/%.s
	$(AS) -c -o $@ $< $(SFLAGS)

$(ODIR):
	@mkdir $@

.PHONY: clean

clean:
	rm -f $(TARGET) $(ODIR)/*.o