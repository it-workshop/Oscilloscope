#include "dac.h"
#include <util/delay.h>
#include <avr/io.h>

void dac_send(uint16_t data) {
	uint8_t x=12;
	uint8_t t;
	DAC_CMDPORT|=DAC_LD|DAC_CLR;
	dac_delay();
	DAC_CMDPORT&=~DAC_CS;
	do {
		x--;
		t=(data&(1<<x))!=0;
		DAC_CMDPORT&=~DAC_CLK;
		if(t) {
			DAC_CMDPORT|=DAC_SDI;
		}
		else {
			DAC_CMDPORT&=~DAC_SDI;
		}
		dac_delay();
		DAC_CMDPORT|=DAC_CLK;
		dac_delay();
	}
	while(x);
	DAC_CMDPORT&=~DAC_SDI;
	DAC_CMDPORT|=DAC_CS;
	dac_delay();
	DAC_CMDPORT&=~DAC_LD;
	dac_delay();
	DAC_CMDPORT|=DAC_LD;
	dac_delay();
}

void dac_init() {
	DAC_CMDDDR|=DAC_CS|DAC_CLK|DAC_SDI|DAC_LD|DAC_CLR;
	DAC_CMDPORT|=DAC_SDI|DAC_CLK|DAC_CS|DAC_CLR;
	DAC_CMDPORT|=DAC_LD;
	DAC_CMDPORT&=~DAC_CLR;
	dac_delay();
	DAC_CMDPORT|=DAC_CLR;
}

void dac_send_16bit(uint16_t data) {
	dac_send(data>>4);
}
