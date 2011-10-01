#ifndef DAC_H
#define DAC_H

#include <avr/io.h>

#define DAC_MIN 0
#define DAC_MAX 0b0000111111111111

//pins
#define DAC_CMDDDR DDRB
#define DAC_CMDPORT PORTB
#define DAC_CS 1
#define DAC_CLK (1<<1)
#define DAC_SDI (1<<2)
#define DAC_LD (1<<3)
#define DAC_CLR (1<<4)
#define dac_delay() asm("nop")

void dac_init();
void dac_send(uint16_t data);
void dac_send_16bit(uint16_t data);

#endif
