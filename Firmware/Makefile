TARGET	= tikkx

CC		= riscv64-unknown-elf-gcc
SIZE	= riscv64-unknown-elf-size
OBJDUMP	= riscv64-unknown-elf-objdump
OBJCOPY	= riscv64-unknown-elf-objcopy

CFLAGS+= \
	-g -Os \
	-flto -ffunction-sections \
	-Wall -Wshadow \
	-march=rv32ec \
	-mabi=ilp32e \
	-nostdlib \
	-static-libgcc \
	-I/usr/include/newlib \
	-Ich32v003fun \
	-I. 

LINKER_SCRIPT?=ch32v003fun/ch32v003fun.ld

LDFLAGS+=-T $(LINKER_SCRIPT) -Wl,--gc-sections -Lch32v003fun -lgcc

CFILES	= $(wildcard *.c) 
SFILES  = $(wildcard *.s)

SOBJS   =   $(SFILES:.s=.o)
COBJS   =   $(CFILES:.c=.o)
OBJS	=   $(SOBJS) $(COBJS) ch32v003fun/ch32v003fun.o

build : ch32v003fun $(TARGET).bin

%.o: %.c
	@echo Compiling $<
	@$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET).elf: $(OBJS)
	@echo Linking $^
	@$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

$(TARGET).bin : $(TARGET).elf
	@$(SIZE) $^
	@echo Creating info files
	@$(OBJDUMP) -S $^ > $(TARGET).lst
	@$(OBJDUMP) -t $^ > $(TARGET).map
	@$(OBJCOPY) -O binary $< $(TARGET).bin
	@$(OBJCOPY) -O ihex $< $(TARGET).hex

$(SOBJS) : $(SFILES)
	 $(AS) $(ASFLAGS) $< -o $@

ch32v003fun:
	@echo Getting fresh copy of ch32v003fun from github
	@rm -rf ch32v003fun tmpfun
	@git clone --depth 1 https://github.com/cnlohr/ch32v003fun.git tmpfun
	@cp tmpfun/misc/libgcc.a tmpfun/ch32v003fun
	@mv tmpfun/ch32v003fun/ . 
	@rm -rf tmpfun
	@$(CC) $(CFLAGS) -c -o ch32v003fun/ch32v003fun.o ch32v003fun/ch32v003fun.c

.PHONY: flash clean distclean

flash: $(TARGET).bin
	minichlink -u
	minichlink -D -w $< flash -b 

clean:
	@$(RM) $(OBJS) $(TARGET).elf $(TARGET).bin $(TARGET).hex $(TARGET).lst $(TARGET).map $(TARGET).hex

distclean: clean
	@$(RM) -rf ch32v003fun/
