#include <avr/pgmspace.h>
#include "util/delay.h"
#include <stdio.h>
#include "draw.h"
#include "buttons.h"
#include "lcd.h"
#include "menu.h"
#include "settings.h"

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
	running,
	input,
	m,u,s;
extern int8_t menu_state;
extern uint16_t adc_error,
	adc_check,
	adc_reset,
	lcd_skip;

void buttons_process() {
	running=pause_state();
	
	if(!pause_state1&&running) {
		adc_timer_play();
	}
	if(pause_state1&&!running) {
		adc_timer_pause();
	}
	
	if(menu_state==MENU_NONE) {
		if(right_pressed()) {
			decr_step(adc_period,ADC_PERIOD_MIN,ADC_PERIOD_MAX,(adc_period>>ADC_PERIOD_SCALE));
			osd();
			osd_delay();
			mode_update();
		}
		else if(left_pressed()) {
			incr_step(adc_period,ADC_PERIOD_MIN,ADC_PERIOD_MAX,(adc_period>>ADC_PERIOD_SCALE));
			osd();
			osd_delay();
			mode_update();
		}
		else if(up_pressed()) {
			decrement(adc_period,ADC_PERIOD_MIN,ADC_PERIOD_MAX);
			osd();
			osd_delay();
			mode_update();
		}
		else if(down_pressed()) {
			increment(adc_period,ADC_PERIOD_MIN,ADC_PERIOD_MAX);
			osd();
			osd_delay();
			mode_update();
		}
		else if(top_pressed()) {
			menu_state=MENU_EXIT;
			redraw_menu=1;
		}
	}
	else if(menu_attoplevel()) {
		if(top_pressed()||right_pressed()||left_pressed()
		 ||up_pressed()||down_pressed()) redraw_menu=1;
		if(top_pressed()) {
			if(menu_state==MENU_EXIT) {
				menu_state=MENU_NONE;
				top_state1=0;
			}
			else {
				menu_state|=1;
			}
		}
		else menu_top_change();
	}
	else {
		if(up_pressed()) {
			increment(menu_state,menu_topstate()+1,menu_second_max());
			redraw_menu=1;
		}
		else if(down_pressed()) {
			decrement(menu_state,menu_topstate()+1,menu_second_max());
			redraw_menu=1;
		}
		else if(top_pressed()) {
			menu_state=menu_topstate();
			redraw_menu=1;
		}
		
		else if(menu_state==MENU_MODE_MODES) {
			if(right_pressed()) {
				increment(mode,MODE_MIN,MODE_MAX);
				redraw_menu=1;
				mode_update();
			}
			else if(left_pressed()) {
				decrement(mode,MODE_MIN,MODE_MAX);
				redraw_menu=1;
				mode_update();
			}
		}
		else if(menu_state==MENU_MODE_UART) {
			if(right_pressed()) {
				lcd_all(0);
				lcd_str("pc mode",0,0);
				lcd_str("you can return\ncontrol from pc or\nreboot the device.",0,2);
				u=127;
				m=127;
				s=127;
				mode=MODE_UART;
				redraw_menu=0;
				mode_update();
			}
		}
		
		else if(menu_state==MENU_ADC_FREQ) {
			if(right_pressed()) {
				incr_step(adc_period,ADC_PERIOD_MIN,ADC_PERIOD_MAX,(adc_period>>ADC_PERIOD_SCALE));
				redraw_menu=1;
			}
			else if(left_pressed()) {
				incr_step(adc_period,ADC_PERIOD_MIN,ADC_PERIOD_MAX,(adc_period>>ADC_PERIOD_SCALE));
				redraw_menu=1;
			}
		}
		else if(menu_state==MENU_ADC_INPUT) {
			if(right_pressed()||left_pressed()) {
				input=input?0:1;
				redraw_menu=1;
				mode_update();
			}
		}
		
		else if(menu_state==MENU_TRIGGER_CHECK) {
			if(right_pressed()) {
				increment(adc_check,0,ALL_N-1);
				redraw_menu=1;
			}
			else if(left_pressed()) {
				decrement(adc_check,0,ALL_N-1);
				redraw_menu=1;
			}
		}
		else if(menu_state==MENU_TRIGGER_ERROR) {
			if(right_pressed()) {
				incr_step(adc_error,0,ADC_ERROR_MAX,ADC_ERROR_STEP);
				redraw_menu=1;
			}
			else if(left_pressed()) {
				decr_step(adc_error,0,ADC_ERROR_MAX,ADC_ERROR_STEP);
				redraw_menu=1;
			}
		}
		else if(menu_state==MENU_TRIGGER_RESET) {
			if(right_pressed()) {
				incr_step(adc_reset,1,ADC_ERROR_MAX,ALL_N);
				redraw_menu=1;
			}
			else if(left_pressed()) {
				decr_step(adc_reset,1,ADC_ERROR_MAX,ALL_N);
				redraw_menu=1;
			}
		}
		
		else if(menu_state==MENU_VIEW_LCDSKIP) {
			if(right_pressed()) {
				incr_step(lcd_skip,LCD_SKIP_MIN,LCD_SKIP_MAX,LCD_SKIP_STEP);
				redraw_menu=1;
			}
			else if(left_pressed()) {
				decr_step(lcd_skip,LCD_SKIP_MIN,LCD_SKIP_MAX,LCD_SKIP_STEP);
				redraw_menu=1;
			}
		}
		else if(menu_state==MENU_VIEW_SPECTRUMYZOOM) {
			if(right_pressed()) {
				increment(spectrum_y_zoom,SPECTRUM_ZOOM_MIN,SPECTRUM_ZOOM_MAX_Y);
				redraw_menu=1;
			}
			if(left_pressed()) {
				decrement(spectrum_y_zoom,SPECTRUM_ZOOM_MIN,SPECTRUM_ZOOM_MAX_Y);
				redraw_menu=1;
			}
		}
	}
	buttons_update();
}



inline void buttons_update() {
	right_state1=right_state();
	left_state1=left_state();
	top_state1=top_state();
	up_state1=up_state();
	down_state1=down_state();
	pause_state1=pause_state();
}
