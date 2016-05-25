ARMGNU ?= arm-none-eabi

CC = $(ARMGNU)-gcc
LD = $(ARMGNU)-ld
AS = $(ARMGNU)-as
OBJCOPY = $(ARMGNU)-objcopy
OBJDUMP = $(ARMGNU)-objdump

LIBPI = ../../../../libpi

CPPFLAGS = -I$(LIBPI)//include
CFLAGS = -std=c99 -Wall -Og -g -ffreestanding $(CPPFLAGS)
ASFLAGS = 
LDFLAGS  = -nostdlib -T memmap -L$(LIBPI)/lib 
LDLIBS = -lpi -lgcc

.SUFFIXES: 

NAME = lab7
SRCS = $(NAME).c cstart.c interrupts.c
OBJS = $(SRCS:.c=.o) vectors.o start.o register.o $(LIBPI)/modules/circular.o

all : $(NAME).bin

start.o: start.s

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@ 

%.o: %.c
	$(CC) $(CFLAGS) -c $< 
	$(CC) -MM $(CFLAGS) -c $< > $*.d

%.elf: start.o cstart.o $(OBJS)
	$(LD) $(LDFLAGS) $^ $(LDLIBS) -o $@ 

%.bin: %.elf
	$(OBJCOPY) $< -O binary $@

%.list: %.o
	$(OBJDUMP) -d --no-show-raw-insn $< > $@

clean :
	rm -f *.bin
	rm -f *.elf
	rm -f *.o
	rm -f *.d
	rm -f *.list

install: $(NAME).bin
	rpi-install.py $(NAME).bin

.PHONY : all clean

.PRECIOUS: %.elf %.o

# include dependency info 
#  "-" suppresses errors
-include $(SRCS:.c=.d)
