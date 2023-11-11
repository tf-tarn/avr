all: build/prog.elf

program: build/prog.elf
	avrdude -C ~/projects/jtag2updi/avrdude.conf -c jtag2updi -P /dev/ttyUSB0 -p t412 -U "flash:w:build/prog.elf:e"

clean:
	rm -Rf build/*

build/%.o: %.c
	$(CC) $(COMPILE_OPTS) -c -o$@ $<

OBJECTS_=$(addprefix build/,$(OBJECTS))
build/prog.elf: $(OBJECTS_)
	$(CC) $(LINK_OPTS) -o$@ $^
