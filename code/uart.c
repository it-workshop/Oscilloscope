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
#include <util/delay.h>
#include "uart.h"
#include "lcd.h"
#include "settings.h"

extern uint8_t m,s,u;
extern uint16_t ymin,ymax,c,c1;
extern uint16_t capture[ALL_N];

extern uint8_t current,
	mode,
	top_state1,
	left_state1,
	right_state1,
	up_state1,
	down_state1,
	pause_state1,
	redraw_menu,
	array_filled,
	error_storage,
	spectrum_x_zoom,
	spectrum_y_zoom,
	menu_state,
	
	trigger_enabled,
	
	mode_update_flag,
	
	running;
extern uint16_t adc_error,
	adc_check,
	adc_reset,
	lcd_skip,
	adc_reset_c;

uint8_t uart_buf,uart_buf_empty=1;

uint8_t action=UART_ACTION_DEFAULT;

void uart_flush() {
	uint8_t uart_tmp;
	while(uart_available()) uart_tmp=UDR;
}

void uart_init() {
	UBRRH = UBRRVAL >> 8;
	UBRRL = UBRRVAL & 0xff;
	UCSRC = UCSRC_SELECT | (1 << UCSZ1) | (1 << UCSZ0);
	UCSRB = (1 << RXEN) | (1 << TXEN) | (1 << RXCIE0) | (1 << TXCIE0);
	uart_flush();
	action=UART_ACTION_DEFAULT;
	uart_buf_empty=1;
}

void uart_putc(uint8_t c) {
	UDR=c;
}

void uart_close() {
	UCSRC=0;
	UCSRB=0;
}

ISR(USART0_TX_vect) {
	if(!uart_buf_empty) {
		uart_buf_empty=1;
		uart_putc(uart_buf);
	}
}

ISR(USART0_RX_vect) {
	uint8_t tmp;
	tmp=UDR;
	//static uint16_t x1=0;
	if(mode==MODE_UART||mode==MODE_UART_BUF) {
		/*lcd_num_from_right(DISPLAY_X-1,3,adc_error);
		lcd_num_from_right(DISPLAY_X-1,4,adc_reset);
		lcd_num_from_right(DISPLAY_X-1,5,adc_check);
		lcd_num_from_right(DISPLAY_X-1,6,action);
		lcd_num_from_right(DISPLAY_X-1,7,x1);
		lcd_sym(tmp,DISPLAY_X/2-1-FONT_SIZE,6);
		x1++;*/
		uart_action(tmp);
	}
}


void uart_action(uint8_t c) {
	static uint8_t c2=0;
	static uint16_t c1;
	if(action==UART_ACTION_DEFAULT) {
		if(c=='c') {
			return_control();
			uart_buf_empty=0;
			return;
		}
		else if(c=='g') {
			if(uart_available_tx()&&uart_buf_empty) {
				if(mode==MODE_UART) {
					uart_putc(capture[0]&0xff);
					uart_buf=capture[0]>>8;
					uart_buf_empty=0;
					return;
				}
				else if(mode==MODE_UART_BUF) {
					if(current>=(ALL_N-1)) {
						adc_timer_pause();
						uart_putc(capture[c2]&0xff);
						uart_buf=capture[c2++]>>8;
						uart_buf_empty=0;
						if(c2>=(ALL_N)) {
							if(!array_filled) array_filled=1;
							adc_reset_c=0;
							current=0;
							c2=0;
						}
						return;
					}
				}
			}
		}
		else if(c=='n') {
			uart_buf_empty=1;
			mode=MODE_UART;
			mode_update_flag=1;
		}
		else if(c=='b') {
			uart_buf_empty=1;
			current=0;
			action=UART_ACTION_B_RC0;
		}
		else if(c=='t') {
			uart_buf_empty=1;
			action=UART_ACTION_TV_RC0;
		}
		else if(c=='r') {
			uart_buf_empty=1;
			action=UART_ACTION_TR_RC0;
		}
		else if(c=='e') {
			uart_buf_empty=1;
			action=UART_ACTION_TE_RC0;
		}
	}
	else if(action==UART_ACTION_TV_RC0) {
		adc_check=c;
		action=UART_ACTION_DEFAULT;
	}
	else if(action==UART_ACTION_TE_RC0) {
		adc_error=c*ADC_ERROR_STEP;
		action=UART_ACTION_DEFAULT;
	}
	else if(action==UART_ACTION_TR_RC0) {
		adc_reset=c*ALL_N;
		action=UART_ACTION_DEFAULT;
	}
	else if(action==UART_ACTION_B_RC0) {
		c1=c;
		action=UART_ACTION_B_RC1;
	}
	else if(action==UART_ACTION_B_RC1) {
		c1|=c<<8;
		adc_period=c1;
		mode=MODE_UART_BUF;
		action=UART_ACTION_DEFAULT;
		trigger_enabled=1;
		mode_update_flag=1;
		trigger_enabled=1;
		current=0;
	}
}
