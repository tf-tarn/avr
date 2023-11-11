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

void adc_init() {
    VREF.CTRLA = VREF_ADC0REFSEL_1V1_gc;

    ADC0.CTRLA = ADC_RESSEL_10BIT_gc;
    // Accumulate multiple samples. These are added to ADCx.RES, so
    // divide by the number of samples to obtain the average.
    ADC0.CTRLB = ADC_SAMPNUM_ACC1_gc;
    ADC0.CTRLC = ADC_REFSEL_INTREF_gc | ADC_PRESC_DIV256_gc;
    ADC0.MUXPOS = ADC_MUXPOS_AIN3_gc;
    ADC0.INTCTRL |= 1 << 0;

    // enable
    ADC0.CTRLA |= 1;
}

volatile unsigned char send = 0;
volatile unsigned char adc_result_available = 0;
volatile uint16_t adc_result = 0;

const char *hex_digits = "0123456789abcdef";

int main (void)
{
    // Use alternate pins (TX on PA1, RX on PA2)
    PORTMUX_CTRLB |= PORTMUX_USART0_bm;

    PORTA.DIRSET = (1 << 2) | (1 << 1);
    PORTA.DIRCLR = (1 << 3);
    PORTA.PIN3CTRL = 0;

    CPU_SREG &= ~CPU_I_bm;
    uart_init();
    CPU_SREG |= CPU_I_bm;

    timer_a_init();
    adc_init();

    while (1) {
        if (send) {
            while (!(USART0.STATUS & USART_DREIF_bm));
            USART0.TXDATAL = 'U';
            send = 0;
        }
        if (adc_result_available) {

            while (!(USART0.STATUS & USART_DREIF_bm));
            USART0.TXDATAL = 'x';
            while (!(USART0.STATUS & USART_DREIF_bm));
            USART0.TXDATAL = hex_digits[(adc_result & 0xf000) >> 12];
            while (!(USART0.STATUS & USART_DREIF_bm));
            USART0.TXDATAL = hex_digits[(adc_result & 0xf00) >> 8];
            while (!(USART0.STATUS & USART_DREIF_bm));
            USART0.TXDATAL = hex_digits[(adc_result & 0xf0) >> 4];
            while (!(USART0.STATUS & USART_DREIF_bm));
            USART0.TXDATAL = hex_digits[adc_result & 0xf];
            while (!(USART0.STATUS & USART_DREIF_bm));
            USART0.TXDATAL = '\r';
            while (!(USART0.STATUS & USART_DREIF_bm));
            USART0.TXDATAL = '\n';
            adc_result_available = 0;
        }
    }

    return 0;

}

ISR(ADC0_RESRDY_vect) {
    adc_result = ADC0.RES;
    adc_result_available = 1;
}

ISR(TCA0_OVF_vect) {
    send = 1;
    PORTA.OUTSET |= (1 << 2);
    TCA0.SINGLE.INTFLAGS |= (1 << 0);

    if (!ADC0.COMMAND) {
        ADC0.COMMAND |= 1;
    }
}

ISR(TCA0_CMP0_vect) {
    PORTA.OUTCLR |= (1 << 2);
    TCA0.SINGLE.INTFLAGS |= (1 << 4);
}
