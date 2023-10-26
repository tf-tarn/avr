#include <avr/io.h>
#include <util/delay.h>

int main (void)
{
    PORTA.DIRSET |= 2;

    while (1) {
        PORTA.OUTCLR |= 2;
        _delay_ms(1000);
        PORTA.OUTSET |= 2;
        _delay_ms(1000);
    }
    return 0;

}
