#include <avr/interrupt.h>

#define BAUD_RATE(R) (uint16_t)((float)(F_CPU * 64 / (16 * (float)(R))) + 0.5)

void uart_init(char async_receive) {
    USART0.BAUD = BAUD_RATE(9600);
    if (async_receive) {
        USART0.CTRLA = USART_RXCIE_bm;
    }
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

void uart_write(char c) {
    while (!(USART0.STATUS & USART_DREIF_bm));
    USART0.TXDATAL = c;
}
