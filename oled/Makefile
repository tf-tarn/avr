COMPILE_OPTS=-mmcu=attiny412 -c -g -Os -Wall -DF_CPU=3333333UL
LINK_OPTS=-mmcu=attiny412 -g -Os -Wall

CC=avr-gcc

OBJECTS=terminal.o oled.o font.o spi.o uart.o main.o

ADDITIONAL_TARGETS=build/rawimage2oled build/bowman.oled

include ../common.mk

build/rawimage2oled: rawimage2oled.c
	gcc -g -Wall -o$@ $^

build/bowman.oled: bowman.data build/rawimage2oled
	build/rawimage2oled < $^ > $@
