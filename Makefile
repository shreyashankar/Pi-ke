# A makefile for building CS107E programs composed of C/asm source files.

# Note this makefile assumes that ../cs107e.github.io is the CS107E
# course repository (so it can find libpi modules). You learn
# how Makefiles work in Lab 4.
CS107E=../cs107e.github.io

# Configure compile options via CFLAGS
#  -I           look in this directory for #include files
#  -g        	compile with debug information
#  -Wall     	give verbose compiler warnings
#  -Og       	apply moderate optimization, retain structure for debugging
#  -std=c99  	use the C99 standard language definition
#  -ffreestanding	do not assume presence of standard library (non-hosted)
CFLAGS = -I$(CS107E)/libpi/include -I./include -g -Wall -Og -std=c99 -ffreestanding

# Your application code: edit to change what is part of your executable
# The source files need to be in the local directory.
override OBJECTS += start.o cstart.o vectors.o halleffect.o i2c.o display.o interrupt_handlers.o gyro.o

# Your library modules: edit to change which code of yours is in libpi
# The source files need to be in the local directory.
LIBPI_STUDENT_MODULES = 

# Reference library modules: edit to change which reference code is in libpi
# These .o files are in ../cs107e.github.io/libpi/modules
override LIBPI_REFERENCE_MODULES += register.o gpioextra.o uart.o memory.o reboot.o strtol.o mailbox.o font.o vectors.o gpioevent.o interrupts.o gpio.o printf.o timer.o armtimer.o

# The first target listed in the makefile is the default product built when
# make is invoked with no argument. So if you type 'make', it'll try to
# run the 'all' recipe, and so build main.bin. You can add test.bin here if
# you also want that to be built upon calling 'make'.
all : main.bin

# main.bin and test.bin depend on the object files, so make will recompile them if needed.
# Next, link object files into an ELF and turn it into binary.
#   -nostdlib   	do not link standard system startup nor default libraries
#   -T memmap           use our linker map
#   -L                  look in this directory for libraries
#   -l                  link this library: -lpi is your libpi, -lgcc is libgcc, for
#                       some low-level support functions like division
main.bin: libpi main.o $(OBJECTS)
	arm-none-eabi-gcc -nostdlib -T memmap $(OBJECTS) main.o -L. -L$(CS107E)/lib -lpi -lgcc -o main.elf
	arm-none-eabi-objcopy main.elf -O binary main.bin

test.bin: libpi test.o $(OBJECTS)
	arm-none-eabi-gcc -nostdlib -T memmap $(OBJECTS) test.o -L. -L$(CS107E)/lib -lpi -lgcc -o test.elf
	arm-none-eabi-objcopy test.elf -O binary test.bin

# These 2 pattern rules tell Make how to compile the .o files
#   $@ means the target (left of colon)
#   $< means the dependencies (right of the colon)
%.o: %.c
	arm-none-eabi-gcc $(CFLAGS) -c $< -o $@
%.o: %.s
	arm-none-eabi-as $< -o $@

# The install target uploads freshly made binary image to rpi bootloader
install: main.bin
	rpi-install.py main.bin

# Since this depends on test.bin, it triggers test.bin to be compiled
# and install the tests binary image to rpi bootloader
test: test.bin
	rpi-install.py test.bin

# The clean target removes previous build products, will force build a-new on next make
clean:
	rm -f *.o *.bin *.elf *.list *~ libpi.a

# These Makefile rules below are for building libpi. It combines all of
# the objects named in LIBPI_REFERENCE_MODULES and LIBPI_STUDENT_MODULES
# into a library libpi.a, then checks whether there is overlap between
# these two lists, which would likely cause problems.

LIBPI_REFERENCE_WITH_PATH = $(patsubst %.o,$(CS107E)/libpi/modules/%.o,$(LIBPI_REFERENCE_MODULES))

# This rule builds libpi. It combines LIB_REFERENCE_MODULES and
# STUDENT_MODULES into libpi.a. It also double-checks whether you
# have included overlapping modules and reports the doubly-implemented
# functions. This is so you don't think your implementation of a
# function is working when in fact you're using the reference.
libpi: $(LIBPI_STUDENT_MODULES)
	@echo "Building libpi"
	@echo "   Including modules: $(LIBPI_REFERENCE_MODULES)"
	arm-none-eabi-ar r libpi.a $(LIBPI_REFERENCE_WITH_PATH) $(LIBPI_STUDENT_MODULES)
	$(CS107E)/libpi/scripts/check-conflicts "$(LIBPI_REFERENCE_WITH_PATH)" "$(LIBPI_STUDENT_MODULES)"

# Targets that aren't filename of build product should be declared "phony" 
.PHONY: all clean libpi

