#include <stdint.h>
#include <avr/io.h>
#include <avr/cpufunc.h>

void spi_write1(uint8_t c) {
    for (char i = 0; i < 8; ++i) {
        if (c & 0x80) {
            PORTA.OUTSET = (1 << 6);
        } else {
            PORTA.OUTCLR = (1 << 6);
        }
        _NOP();
        PORTA.OUTSET = (1 << 3);
        _NOP();
        PORTA.OUTCLR = (1 << 3);
        c <<= 1;
    }

    PORTA.OUTCLR = (1 << 6);
}

/* This works, but it leaves the SS and MISO pins unusable.
void spi_write1(uint8_t c) {
    while (!(SPI0.INTFLAGS & SPI_DREIF_bm));
    SPI0.DATA = c;
}
*/
