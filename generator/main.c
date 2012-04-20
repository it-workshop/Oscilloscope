#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "dac.h"

extern uint8_t N;
extern uint16_t values[];

uint8_t c;

ISR(TIMER1_COMPB_vect)
{
    TCNT1 = 0x0000;
    dac_send(values[c]);
    if(c >= (N-1)) c = 0;
    else c++;
}

int main()
{
    c = 0;
    dac_init();

    TCCR1A = 0;
    TCNT1 = 0x0000;
    OCR1B = 0x0001;
    
    TIMSK1 |= (1 << OCIE1B);
    TCCR1B = 0x001;
    
    sei();
    
    for(;;);
    
    return(0);
}
