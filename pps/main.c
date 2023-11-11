#include <avr/interrupt.h>
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

void timer_a_init() {
    // 3333333 / 1024
    TCA0.SINGLE.PER = 3255;
    TCA0.SINGLE.CTRLA = TCA_SPLIT_CLKSEL_DIV1024_gc;
    TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc;
    // 10% duty cycle
    TCA0.SINGLE.CMP0 = 326;
    // enable OVF, CMP0 interrupts
    TCA0.SINGLE.INTCTRL |= (1 << 0) | (1 << 4);


    // enable
    TCA0.SINGLE.CTRLA |= 1;
}

volatile char send = 0;

int main (void)
{
    // Use alternate pins (TX on PA1, RX on PA2)
    PORTMUX_CTRLB |= PORTMUX_USART0_bm;

    PORTA.DIRSET |= (1 << 2) | (1 << 1);

    CPU_SREG &= ~CPU_I_bm;
    uart_init();
    CPU_SREG |= CPU_I_bm;

    timer_a_init();

    while (1) {
        if (send) {
            while (!(USART0.STATUS & USART_DREIF_bm));
            USART0.TXDATAL = 'U';
            send = 0;
        }
    }
        
    
    
    return 0;

}

ISR(TCA0_OVF_vect) {
    send = 1;
    PORTA.OUTSET |= (1 << 2);
    TCA0.SINGLE.INTFLAGS |= (1 << 0);
}

ISR(TCA0_CMP0_vect) {
    PORTA.OUTCLR |= (1 << 2);
    TCA0.SINGLE.INTFLAGS |= (1 << 4);
}
