#include <avr/pgmspace.h>
#include "util/delay.h"
#include <stdio.h>
#include "draw.h"
#include "buttons.h"
#include "lcd.h"
#include "menu.h"
#include "settings.h"

#ifndef MENU_C
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
	input;
extern int8_t menu_state;
extern uint16_t adc_error,
	adc_check,
	adc_reset,
	lcd_skip;
#endif

void buttons_process() {
	running=pause_state();
	if(!pause_state1&&running) {
		adc_timer_play();
	}
	if(pause_state1&&!running) {
		adc_timer_pause();
	}
	if(menu_state==-1) {
		if(!right_state()&&right_state1) {
			incr_step(adc_period,ADC_PERIOD_MIN,ADC_PERIOD_MAX,adc_step);
			osd();
			osd_delay();
			mode_update();
		}
		else if(!left_state()&&left_state1) {
			decr_step(adc_period,ADC_PERIOD_MIN,ADC_PERIOD_MAX,adc_step);
			osd();
			osd_delay();
			mode_update();
		}
		else if(!up_state()&&up_state1) {
			increment(adc_period,ADC_PERIOD_MIN,ADC_PERIOD_MAX);
			osd();
			osd_delay();
			mode_update();
		}
		else if(!down_state()&&down_state1) {
			decrement(adc_period,ADC_PERIOD_MIN,ADC_PERIOD_MAX);
			osd();
			osd_delay();
			mode_update();
		}
		else if(!top_state()&&top_state1) {
			menu_state=0;
			redraw_menu=1;
		}
	}
	else if(menu_state>MENU_MAX) {
		if(!top_state()&&(top_state1)) {
			menu_state-=MENU_MAX;
			redraw_menu=1;
		}
		else if(menu_state==(MENU_MAX+MENU_MODE)) {
			if(!right_state()&&right_state1) {
				increment(mode,MODE_MIN,MODE_MAX);
				if(mode!=MODE_XY) ADMUX=0b01100000;
				redraw_menu=1;
				mode_update();
			}
			else if(!left_state()&&left_state1) {
				decrement(mode,MODE_MIN,MODE_MAX);
				if(mode!=MODE_XY) ADMUX=0b01100000;
				redraw_menu=1;
				mode_update();
			}
		}
		else if(menu_state==(MENU_MAX+MENU_ADCPERIOD)) {
			if(!right_state()&&right_state1) {
				increment(adc_period,ADC_PERIOD_MIN,ADC_PERIOD_MAX);
				redraw_menu=1;
			}
			else if(!left_state()&&left_state1) {
				decrement(adc_period,ADC_PERIOD_MIN,ADC_PERIOD_MAX);
				redraw_menu=1;
			}
		}
		else if(menu_state==(MENU_MAX+MENU_ADCCHECK)) {
			if(!right_state()&&right_state1) {
				increment(adc_check,0,ALL_N-1);
				redraw_menu=1;
			}
			else if(!left_state()&&left_state1) {
				decrement(adc_check,0,ALL_N-1);
				redraw_menu=1;
			}
		}
		else if(menu_state==(MENU_MAX+MENU_ADCERROR)) {
			if(!right_state()&&right_state1) {
				incr_step(adc_error,0,ADC_ERROR_MAX,ADC_ERROR_STEP);
				redraw_menu=1;
			}
			else if(!left_state()&&left_state1) {
				decr_step(adc_error,0,ADC_ERROR_MAX,ADC_ERROR_STEP);
				redraw_menu=1;
			}
		}
		else if(menu_state==(MENU_MAX+MENU_ADCRESET)) {
			if(!right_state()&&right_state1) {
				incr_step(adc_reset,1,ADC_ERROR_MAX,ALL_N);
				redraw_menu=1;
			}
			else if(!left_state()&&left_state1) {
				decr_step(adc_reset,1,ADC_ERROR_MAX,ALL_N);
				redraw_menu=1;
			}
		}
		else if(menu_state==(MENU_MAX+MENU_ADCSTEP)) {
			if(!right_state()/*&&right_state1*/) {
				increment(adc_step,ADC_STEP_MIN,ADC_PERIOD_MAX);
				redraw_menu=1;
			}
			else if(!left_state()/*&&left_state1*/) {
				decrement(adc_step,ADC_STEP_MIN,ADC_PERIOD_MAX);
				redraw_menu=1;
			}
		}
		else if(menu_state==(MENU_MAX+MENU_LCDSKIP)) {
			if(!right_state()&&right_state1) {
				incr_step(lcd_skip,LCD_SKIP_MIN,LCD_SKIP_MAX,LCD_SKIP_STEP);
				redraw_menu=1;
			}
			else if(!left_state()&&left_state1) {
				decr_step(lcd_skip,LCD_SKIP_MIN,LCD_SKIP_MAX,LCD_SKIP_STEP);
				redraw_menu=1;
			}
		}
		else if(menu_state==(MENU_MAX+MENU_SPECTRUMXZOOM)) {
			if(!right_state()&&right_state1) {
				increment(spectrum_x_zoom,SPECTRUM_ZOOM_MIN,SPECTRUM_ZOOM_MAX_X);
				redraw_menu=1;
			}
			if(!left_state()&&left_state1) {
				decrement(spectrum_x_zoom,SPECTRUM_ZOOM_MIN,SPECTRUM_ZOOM_MAX_X);
				redraw_menu=1;
			}
		}
		else if(menu_state==(MENU_MAX+MENU_SPECTRUMYZOOM)) {
			if(!right_state()&&right_state1) {
				increment(spectrum_y_zoom,SPECTRUM_ZOOM_MIN,SPECTRUM_ZOOM_MAX_Y);
				redraw_menu=1;
			}
			if(!left_state()&&left_state1) {
				decrement(spectrum_y_zoom,SPECTRUM_ZOOM_MIN,SPECTRUM_ZOOM_MAX_Y);
				redraw_menu=1;
			}
		}
		else if(menu_state==(MENU_MAX+MENU_INPUTSELECT)) {
			if((!right_state()&&right_state1)||(!left_state()&&left_state1)) {
				input=input?0:1;
				redraw_menu=1;
				mode_update();
			}
		}
	}
	else {
		if(!right_state()&&right_state1) {
			menu_state=(menu_state==MENU_MAX)?0:menu_state+1;
			redraw_menu=1;
		}
		if(!left_state()&&left_state1) {
			menu_state=menu_state?menu_state-1:MENU_MAX;
			redraw_menu=1;
		}
		if(!top_state()&&(top_state1)) {
			if(menu_state==0) {
				menu_state=-1;
				redraw_menu=1;
				top_state1=0;
			}
			else {
				menu_state+=MENU_MAX;
				top_state1=0;
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
