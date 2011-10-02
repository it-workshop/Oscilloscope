#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "dac.h"
#include <math.h>

uint8_t c;

uint16_t values[]={2048,2304,2557,2801,3034,3251,3449,3625,3776,3900,3995,4059,4091,4091,4059,3995,3900,3776,3625,3449,3251,3034,2801,2557,2304,2047,1791,1538,1294,1061,844,646,470,319,195,100,36,4,4,36,100,195,319,470,646,844,1061,1294,1538,1791};

ISR(TIMER1_COMPB_vect) {
	TCNT1=0x0000;
	dac_send(values[c]);
	if(c>=49) c=0;
	else c++;
}

int main() {
	c=0;
	dac_init();

	TCCR1A=0;
	TCNT1=0x0000;
	OCR1B=1;
	
	TIMSK1|=(1<<OCIE1B);
	TCCR1B=0x001;
	
	sei();
	
	for(;;);
	
	return;
}
