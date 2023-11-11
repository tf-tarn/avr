#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#define BAUD_RATE(R) (uint16_t)((float)(F_CPU * 64 / (16 * (float)(R))) + 0.5)

#define TXBUFLEN (1 << 6)
#define TXBUFLEN_MASK (TXBUFLEN - 1)

volatile uint8_t txbuf[TXBUFLEN];
volatile uint8_t txbuf_start = 0;
volatile uint8_t txbuf_end = 0;

void uart_write_(uint8_t c) {
    if (
        (txbuf_start - txbuf_end == 1)
        ||
        (txbuf_start == 0 && txbuf_end == TXBUFLEN - 1)
        ) {
        return;
    }

    txbuf[txbuf_end] = c;
    txbuf_end = (txbuf_end + 1u) & TXBUFLEN_MASK;
}

void uart_write(uint8_t c) {
    USART0.CTRLA &= ~USART_DREIE_bm;
    uart_write_(c);
    USART0.CTRLA |= USART_DREIE_bm;
}

void uart_write_s(const char *s) {
    USART0.CTRLA &= ~USART_DREIE_bm;
    for (; *s; ++s) {
        uart_write_(*s);
    }
    USART0.CTRLA |= USART_DREIE_bm;
}

void uart_init_async() {
    USART0.BAUD = BAUD_RATE(9600);

    USART0.CTRLA = 0;

    USART0.CTRLC =
        USART_CMODE_ASYNCHRONOUS_gc
        | USART_PMODE_DISABLED_gc
        | USART_SBMODE_1BIT_gc
        | USART_CHSIZE_8BIT_gc;
    USART0.CTRLB =
        USART_TXEN_bm
        | USART_RXEN_bm
        | USART_RXMODE_NORMAL_gc
        ;
}

int main (void)
{
    // Use alternate pins (TX on PA1, RX on PA2)
    PORTMUX_CTRLB |= PORTMUX_USART0_bm;

    PORTA.DIRSET |= (1 << 1) | (1 << 6);

    CPU_SREG &= ~CPU_I_bm;
    uart_init_async();
    CPU_SREG |= CPU_I_bm;

    char count = 'a';

    while (1) {
        uart_write(count);
        uart_write_s("\r\nThis is a rather long string.\r\n");
        _delay_ms(200);
        PORTA.OUTCLR = (1 << 6);

        ++count;

        if (USART0.STATUS & USART_RXCIF_bm) {
            PORTA.OUTSET = (1 << 6);
            unsigned char c = USART0.RXDATAL;

            if ('a' <= c && c <= 'z') {
                count = c;
            }
        }

        if (count > 'z') {
            count = 'a';
        }
    }
        
    
    
    return 0;

}

ISR(USART0_DRE_vect) {
    if (txbuf_start == txbuf_end) {
        USART0.CTRLA &= ~USART_DREIE_bm;
        return;
    }
    USART0.TXDATAL = txbuf[txbuf_start];
    txbuf_start = (txbuf_start + 1u) & TXBUFLEN_MASK;
}
