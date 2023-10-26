COMPILE_OPTS=-mmcu=attiny412 -c -g -Os -Wall -DF_CPU=3333333UL
LINK_OPTS=-mmcu=attiny412 -g -Os -Wall

CC="avr-gcc"

all: build/prog.elf

program:
	avrdude -C ~/projects/jtag2updi/avrdude.conf -c jtag2updi -P /dev/ttyUSB1 -p t412 -U "flash:w:build/prog.elf:e"

clean:
	rm -Rf build/*

# For producing assembly output
# build/assembly.S:
# 	$(CC) $(COMPILE_OPTS) -S -obuild/assembly.S main.c uart.c

build/%.o: %.c
	$(CC) $(COMPILE_OPTS) -c -o$@ $<

build/prog.elf: build/main.o
	$(CC) $(LINK_OPTS) -o$@ $^




# --memorysummary dist/default/production/memoryfile.xml ??
