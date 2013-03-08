#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "dac.h"
#include "function.h"

//list of ports (multiple dac)
extern volatile unsigned char* DAC_CMDDDRS[];
extern volatile unsigned char* DAC_CMDPORTS[];

//current port to send data to
volatile unsigned char* DAC_CMDDDR; // current DDR
volatile unsigned char* DAC_CMDPORT; // current PORT

//samples array
extern uint16_t values[DAC_N][N];

uint8_t DAC_CN = 0; // current DAC number in array
uint8_t c; //current sample

ISR(TIMER1_COMPB_vect)
{
    TCNT1 = 0x0000;
    for(DAC_CN = 0; DAC_CN < DAC_N; DAC_CN++)
	{
		DAC_CMDDDR = DAC_CMDDDRS[DAC_CN];
		DAC_CMDPORT = DAC_CMDPORTS[DAC_CN];
		dac_send(values[DAC_CN][c]);
	}
    if(c >= (N - 1)) c = 0;
    else c++;
}

int main()
{
    c = 0;
    
    for(DAC_CN = 0; DAC_CN < DAC_N; DAC_CN++)
	{
		DAC_CMDDDR = DAC_CMDDDRS[DAC_CN];
		DAC_CMDPORT = DAC_CMDPORTS[DAC_CN];
		dac_init();
	}

    TCCR1A = 0;
    TCNT1 = 0x0000;
    OCR1B = 0x0001;
    
    TIMSK1 |= (1 << OCIE1B);
    TCCR1B = 0x001;
    
    sei();
    
    for(;;);
    
    return(0);
}
