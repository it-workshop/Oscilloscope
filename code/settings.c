#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sfr_defs.h>
#include <avr/sleep.h>
#include "settings.h"
#include "lcd.h"
#include "uart.h"
#include "menu.h"

extern uint8_t current,
	mode,
	top_state1,
	left_state1,
	right_state1,
	up_state1,
	down_state1,
	pause_state1,
	redraw_menu,
	adc_step,
	clear_screen,
	array_filled,
	error_storage,
	spectrum_x_zoom,
	spectrum_y_zoom,
	running,
	menu_state,
	input;
extern uint16_t adc_error,
	adc_check,
	adc_reset,
	lcd_skip;

void mode_update() {
	//cli();
	if((mode==MODE_UART_BUF)||(mode==MODE_UART)) {
		adc_check=0;
		redraw_menu=0;
		buttons_timer_pause();
	}
	else {
		buttons_timer_play();
		redraw_menu=1;
		clear_screen=1;
	}
	if(mode==MODE_XY) {
			adc_freq_fast();
	}
	else {
		if(input==0) {
			adc_first();
		}
		else if(input==1) {
			adc_second();
		}
		//adc_freq_normal();
		adc_freq_fast();
	}
	if(mode==MODE_UART||mode==MODE_VOLTAGE) {
		adc_timer_pause();
		ADCSRA&=~(1<<ADIE);
		if(mode==MODE_VOLTAGE) {
			buttons_timer_play();
		}
	}
	else {
		running=1;
		ADCSRA|=(1<<ADIE);
		if(mode!=MODE_UART_BUF) {
			buttons_timer_play();
		}
		if(running) {
			adc_timer_play();
		}
	}
	current=0;
}

inline void return_control() {
	mode=MODE_DUAL;
	menu_state=MENU_NONE;
	lcd_all(0);
	mode_update();
}
