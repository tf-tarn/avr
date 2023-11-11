#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "uart.h"
#include "oled.h"
#include "terminal.h"

int main (void)
{
    cli();

    // Use alternate pins (TX on PA1, RX on PA2)
    // Use alternate pins (SCK on PA3, MOSI on PA6)
    PORTMUX_CTRLB = PORTMUX_SPI0_ALTERNATE_gc | PORTMUX_USART0_ALTERNATE_gc;

    PORTA.DIRSET =
        //(1 << 7) // LED
        (1 << 0) // reset
        | (1 << 2) // data/command
        | (1 << 6) | (1 << 3) // SPI
        | (1 << 1); // UART

    uart_init();

    /* PORTA.OUTCLR = (1 << 2); */
    _delay_ms(500);
    uart_write('\r'); uart_write('\n');
    _delay_ms(500);
    uart_write('\r'); uart_write('\n');
    _delay_ms(500);
    uart_write('\r'); uart_write('\n');
    _delay_ms(500);
    uart_write('r'); uart_write('\r'); uart_write('\n');

    PORTA.OUTCLR = (1 << 0);
    _delay_ms(1000);
    PORTA.OUTSET = (1 << 0);
        
    uart_write('h'); uart_write('\r'); uart_write('\n');
    
    /* oled_spi_init(); */
    oled_init();

    uart_write('s'); uart_write('\r'); uart_write('\n');

    term_clear();
    term_write('a');
    /* term_write('a'); */
    /* term_write('a'); */
    /* term_write('a'); */
    /* term_write('a'); */

    uart_write('-'); uart_write('\r'); uart_write('\n');

    char count = 'a';
    while (1) {
        uart_write('.');
        term_write(count);
        _delay_ms(10);
        ++count;
        if (count > 'z') {
            count = 'a';
        }
    }
    return 0;

}
