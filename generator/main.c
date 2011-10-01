#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "dac.h"
#include <math.h>

#define STEP 2.*M_PI/65535.

uint16_t c;

void signal_change() {
	//dac_send((sin(c*STEP)+1)*(4000));
	dac_send(0xffff);
	//dac_send(0x0000);
	c++;
}

ISR(TIMER1_COMPB_vect) {
	TCNT1=0;
	dac_send(0xffff);
	//signal_change();
}

uint16_t values[]={2048,2304,2557,2801,3034,3251,3449,3625,3776,3900,3995,4059,4091,4091,4059,3995,3900,3776,3625,3449,3251,3034,2801,2557,2304,2047,1791,1538,1294,1061,844,646,470,319,195,100,36,4,4,36,100,195,319,470,646,844,1061,1294,1538,1791};

int main() {
	cli();
	c=0;
	dac_init();
/*
	TCNT1=0;
	
	TIMSK1=(1<<OCIE1B);
	TCCR1B=0b001;
	OCR1B=0x0f;*/
	for(c=0;;c++) {
		//dac_send((sin((2*M_PI/7.)*c)+1)*(DAC_MAX/2));
		dac_send(values[c]);
		if(c==49) c=0;
		//_delay_us(1);
	}
	//sei();

	return;
}
