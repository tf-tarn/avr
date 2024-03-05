This is a collection of programs for the ATtiny412. They might run on
other AVR processors. They are to be regarded as proofs of concept, or
minimal working examples of specific functions. They are not to be
considered polished code.

adc/               reading the ADC
blink/             just blink an LED
oled/              small program that can display text and graphics on
                   an OLED display
pcb-devboard/      a small dev/breakout board for the ATtiny412,
                   implemented in kicad, with JLCPCB part numbers
                   for PCBA
pps/               an exercise in using timer interrupts
uart/              blocking IO with the UART
uart_interrupt/    non-blocking IO with the UART
common.mk          common makefile rules, included in makefiles in
                   subdirectories
README.txt         this file
