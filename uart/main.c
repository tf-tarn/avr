#include <avr/io.h>
#include <util/delay.h>

#define BAUD_RATE(R) (uint16_t)((float)(F_CPU * 64 / (16 * (float)(R))) + 0.5)

void uart_init() {
    USART0.BAUD = BAUD_RATE(9600);
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

    PORTA.DIRSET |= 2;

    CPU_SREG &= ~CPU_I_bm;
    uart_init();
    /* CPU.SREG = 1; */

    char count = 'a';

    while (1) {
        while (!(USART0.STATUS & USART_DREIF_bm));
        USART0.TXDATAL = count;
        _delay_ms(200);

        ++count;

        if (USART0.STATUS & USART_RXCIF_bm) {
            unsigned char c = USART0.RXDATAL;

            if ('a' <= c && c <= 'z') {
                count = c;
            }
        } else if (count > 'z') {
            count = 'a';
        }

    }
        
    
    
    return 0;

}
