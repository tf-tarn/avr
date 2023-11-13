#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "uart.h"
#include "oled.h"
#include "terminal.h"

#define RXBUFLEN (1 << 4)
#define RXBUFLEN_MASK (RXBUFLEN - 1)

volatile char rxbuf[RXBUFLEN];
volatile unsigned char rxbuf_start = 0;
volatile unsigned char rxbuf_end = 0;


enum {
    M_TEXT,
    M_GFX
};

struct {
    uint8_t row;
    uint8_t col;
} gfx;

static void set_row(uint8_t r) {
    oled_command(0x22); // set page address
    oled_command(r);
    oled_command(r);
}
static void return_carriage() {
    oled_command(0x21); // set column address
    oled_command(0);
    oled_command(SCREENWIDTH - 1);
}

void gfx_begin() {
    gfx.row = 0;
    gfx.col = 0;
    set_row(0);
    return_carriage();
}

char gfx_draw(uint8_t c) {
    if (gfx.col > SCREENWIDTH - 1) {
        gfx.col = 1;
        ++gfx.row;
        if (gfx.row >= SCREENHEIGHT / 8) {
            // graphics drawing has finished
            return 1;
        }
        set_row(gfx.row);
        return_carriage();
    } else {
        ++gfx.col;
    }

    oled_data(c);

    return 0;
}

int main (void)
{
    char mode = M_GFX;

    cli();

    // Use alternate pins (TX on PA1, RX on PA2)
    // Use alternate pins (SCK on PA3, MOSI on PA6)
    PORTMUX_CTRLB = PORTMUX_SPI0_ALTERNATE_gc | PORTMUX_USART0_ALTERNATE_gc;

    PORTA.DIRSET =
        (1 << 0) // reset
        | (1 << 7) // data/command
        | (1 << 6) | (1 << 3) // SPI
        | (1 << 1); // UART

    uart_init();

    _delay_ms(100);
    PORTA.OUTCLR = (1 << 0);
    _delay_ms(100);
    PORTA.OUTSET = (1 << 0);

    oled_init();
    term_clear();
    sei();

    _delay_ms(100);
    term_write_s("Hello\r\nReady to receive\r\ngraphics data.");
    gfx_begin();

    unsigned char c = 'U';
    while (1) {

        /* cli(); */
        USART0.CTRLA &= ~USART_RXCIE_bm;
        if (rxbuf_start == rxbuf_end) {
            USART0.CTRLA |= USART_RXCIE_bm;
            continue;
        }
        c = rxbuf[rxbuf_start];
        rxbuf_start = (rxbuf_start + 1u) & RXBUFLEN_MASK;
        /* sei(); */
        USART0.CTRLA |= USART_RXCIE_bm;

        if (mode == M_TEXT) {
            switch (c) {
            case 0x3:
                term_clear();
                break;
            case 0x4:
                mode = M_GFX;
                gfx_begin();
                break;
            default:
                term_write(c);
            }
        } else if (mode == M_GFX) {
            if (gfx_draw(c)) {
                mode = M_TEXT;
            }
        }
        _delay_us(300);
    }
    return 0;

}

ISR(USART0_RXC_vect) {
    /* if ((((rxbuf_end + RXBUFLEN) - rxbuf_start) & RXBUFLEN_MASK) < (RXBUFLEN - 2)) { */
    /*     // drop a byte.. oops. */
    /*     return; */
    /* } */
    rxbuf[rxbuf_end] = USART0.RXDATAL;
    rxbuf_end = (rxbuf_end + 1u) & RXBUFLEN_MASK;
}
