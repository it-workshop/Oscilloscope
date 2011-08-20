
/*
 * Copyright (c) 2006-2011 by Roland Riegel <feedback@roland-riegel.de>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sfr_defs.h>
#include <avr/sleep.h>


#ifdef UDR0
#define UBRRH UBRR0H
#define UBRRL UBRR0L
#define UDR UDR0

#define UCSRA UCSR0A
#define UDRE UDRE0
#define RXC RXC0

#define UCSRB UCSR0B
#define RXEN RXEN0
#define TXEN TXEN0
#define RXCIE RXCIE0

#define UCSRC UCSR0C
#define URSEL 
#define UCSZ0 UCSZ00
#define UCSZ1 UCSZ01
#define UCSRC_SELECT 0
#else
#define UCSRC_SELECT (1 << URSEL)
#endif


#define BAUD 9600UL
#define UBRRVAL (F_CPU/(BAUD*16)-1)
#define U_M	1000
uint16_t u_c;
void uart_init() {
    UBRRH = UBRRVAL >> 8;
    UBRRL = UBRRVAL & 0xff;
    UCSRC = UCSRC_SELECT | (1 << UCSZ1) | (1 << UCSZ0);
    UCSRB = (1 << RXEN) | (1 << TXEN);
}

void uart_putc(uint8_t c) {
	u_c=0;
    while(!(UCSRA&(1<<UDRE))&&(u_c<U_M)) {
		u_c++;
	}
    UDR = c;
}

uint8_t uart_getc() {
	u_c=0;
	static uint8_t c;
    while(!(UCSRA & (1 << RXC))&&(u_c<U_M)) {
		u_c++;
	}
	c=UDR;
    return c;
}

void uart_flush( void ) {
  static unsigned char dummy;
  while ( UCSRA & (1<<RXC) ) dummy = UDR;
}

void uart_communicate(unsigned char w_byte) {
    static uint8_t c;
    c=uart_getc();
    if(c=='g') uart_putc(w_byte);
    uart_flush();
}
