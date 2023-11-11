COMPILE_OPTS=-mmcu=attiny412 -c -g -Os -Wall -DF_CPU=3333333UL
LINK_OPTS=-mmcu=attiny412 -g -Os -Wall

CC="avr-gcc"

all: build/prog.elf build/monitor

program: build/prog.elf
	avrdude -C ~/projects/jtag2updi/avrdude.conf -c jtag2updi -P /dev/ttyUSB0 -p t412 -U "flash:w:build/prog.elf:e"

clean:
	rm -Rf build/*

build/%.o: %.c
	$(CC) $(COMPILE_OPTS) -c -o$@ $<

build/prog.elf: build/main.o
	$(CC) $(LINK_OPTS) -o$@ $^

build/monitor: monitor.c
	gcc -g -Wall -o$@ $^
