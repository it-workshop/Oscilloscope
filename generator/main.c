#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "dac.h"
#include <math.h>

uint8_t c;

uint8_t N = 50;
uint16_t values[] = {4095,3931,3767,3604,3440,3276,3112,2948,2785,2621,2457,2293,2129,1966,1802,1638,1474,1310,1147,983,819,655,491,328,164,0,164,328,491,655,819,983,1147,1310,1474,1638,1802,1966,2129,2293,2457,2621,2785,2948,3112,3276,3440,3604,3767,3931};

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
