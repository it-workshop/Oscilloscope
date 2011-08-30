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
	array_filled,
	error_storage,
	spectrum_x_zoom,
	spectrum_y_zoom,
	running,
	menu_state;
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
		redraw_menu=1;
		lcd_all(0);
	}
	if(mode==MODE_UART) {
		adc_timer_pause();
		ADCSRA&=~(1<<ADIE);
	}
	else {
		if(mode==MODE_XY) {
			adc_freq_fast();
		}
		else {
			adc_freq_normal();
		}
		array_filled=0;
		running=1;
		ADCSRA|=(1<<ADIE);
		if(mode!=MODE_UART_BUF) {
			buttons_timer_play();
		}
		adc_timer_play();
	}
	current=0;
	//sei();
}

inline void return_control() {
	mode=MODE_DUAL;
	menu_state=-1;
	lcd_all(0);
	mode_update();
}
