#include <avr/pgmspace.h>
#include <util/delay.h>
#include "menu.h"
#include "lcd.h"
#include "draw.h"
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
	menu_state,
	input;
extern uint16_t adc_error,
	adc_check,
	adc_reset,
	lcd_skip;

void menu_top_change() {
	if(right_pressed()) {
		if(menu_state>=MENU_MAX) menu_state=MENU_EXIT;
		else menu_state=((menu_state>>MENU_SHIFT)+1)<<MENU_SHIFT;
	}
	else if(left_pressed()) {
		if(menu_state<=MENU_EXIT) menu_state=MENU_MAX;
		else menu_state=((menu_state>>MENU_SHIFT)-1)<<MENU_SHIFT;
	}
}

uint8_t menu_second_max() {
	if(menu_topstate()==MENU_MODE) return(MENU_MODE_MAX);
	if(menu_topstate()==MENU_VIEW) return(MENU_VIEW_MAX);
	if(menu_topstate()==MENU_ADC) return(MENU_ADC_MAX);
	if(menu_topstate()==MENU_TRIGGER) return(MENU_TRIGGER_MAX);
}

void draw_menu() {
	if(menu_state!=MENU_NONE) {
		uint8_t topstate_x,topstate1_x,topstate_y,mtopstate=menu_topstate(),state_x;

		if(menu_attoplevel()) {
			topstate_x=(FONT_SIZE+1)*2;
			topstate_y=1;
			lcd_str("menu",0,0);
			lcd_arrows(0,1);
		}
		else {
			topstate_x=0;
			topstate_y=0;
			topstate1_x=FONT_SIZE+1;
			state_x=2*(FONT_SIZE+1);
			lcd_sym(SYM_UPDOWN,0,1);
			if(menu_state!=MENU_MODE_UART&&
			 menu_state!=(MENU_ABOUT|1)) lcd_arrows(0,2);
		}
		if(mtopstate==MENU_EXIT) lcd_str("exit",topstate_x,topstate_y);
		else if(mtopstate==MENU_MODE) lcd_str("mode",topstate_x,topstate_y);
		else if(mtopstate==MENU_ADC) lcd_str("adc",topstate_x,topstate_y);
		else if(mtopstate==MENU_TRIGGER) lcd_str("trigger",topstate_x,topstate_y);
		else if(mtopstate==MENU_VIEW) lcd_str("view",topstate_x,topstate_y);
		else if(mtopstate==MENU_ABOUT) lcd_str("about",topstate_x,topstate_y);
		
		if(menu_state==(MENU_ABOUT|1)) {
			welcome();
			_delay_ms(1000);
			menu_state=menu_topstate();
			redraw_menu=1;
		}
		
		else if(menu_state==MENU_MODE_MODES) {
			lcd_str("modes",topstate1_x,1);
			if(mode==MODE_SIGNAL) lcd_str("signal",state_x,2);
			else if(mode==MODE_SPECTRUM) lcd_str("spectrum",state_x,2);
			else if(mode==MODE_XY) lcd_str("xy",state_x,2);
			else if(mode==MODE_DUAL) lcd_str("dual",state_x,2);
		}
		else if(menu_state==MENU_MODE_UART) {
			lcd_str("pc",topstate1_x,1);
			lcd_str(">connect",0,2);
		}
		
		else if(menu_state==MENU_ADC_FREQ) {
			lcd_str("freq",topstate1_x,1);
			lcd_str("hz",(DISPLAY_X/2)-2*(FONT_SIZE+1),2);
			dfreq_only((DISPLAY_X/2)-1-2*(FONT_SIZE+1),2);
		}
		else if(menu_state==MENU_ADC_INPUT) {
			lcd_str("input",topstate1_x,1);
			if(input==0) lcd_str("first",state_x,2);
			else if(input==1) lcd_str("second",state_x,2);
		}
		
		else if(menu_state==MENU_TRIGGER_CHECK) {
			lcd_str("check/enb.",topstate1_x,1);
			if(adc_check) {
				lcd_num_from_right((DISPLAY_X/2-1),2,adc_check);
			}
			else {
				lcd_str("disabled",state_x,2);
			}
		}
		else if(menu_state==MENU_TRIGGER_ERROR) {
			lcd_str("error",topstate1_x,1);
			lcd_num_from_right((DISPLAY_X/2-1),2,adc_error);
		}
		else if(menu_state==MENU_TRIGGER_RESET) {
			lcd_str("reset",topstate1_x,1);
			lcd_num_from_right((DISPLAY_X/2-1),2,adc_reset);
		}
		
		else if(menu_state==MENU_VIEW_LCDSKIP) {
			lcd_str("lcd skip",topstate1_x,1);
			lcd_num_from_right((DISPLAY_X/2-1),2,lcd_skip);
		}
		else if(menu_state==MENU_VIEW_SPECTRUMYZOOM) {
			lcd_str("sp. zoom",topstate1_x,1);
			lcd_num_from_right((DISPLAY_X/2-1),2,1<<spectrum_y_zoom);
		}
		
		/*if(menu_state==(MENU_MAX+MENU_ADCPERIOD)) {
			lcd_str("adc period",0,0);
			lcd_pixel_line_from_left(1,(adc_period-ADC_PERIOD_MIN)>>5);
			lcd_num_from_right((DISPLAY_X)/2,3,adc_period);
			lcd_arrows(0,2);
		}
		
		if(menu_state==(MENU_MAX+MENU_LCDSKIP)) {
			lcd_str("lcd skip",0,0);
			lcd_pixel_line_from_left(1,lcd_skip>>4);
			lcd_arrows(0,2);
		}
		if(menu_state==(MENU_MAX+MENU_ADCSTEP)) {
			lcd_str("adc p step",0,0);
			lcd_num_from_right((DISPLAY_X/2),1,adc_step);
			lcd_arrows(0,2);
		}
		if(menu_state==(MENU_MAX+MENU_ADCERROR)) {
			lcd_str("adc error",0,0);
			lcd_num_from_right((DISPLAY_X/2),1,adc_error);
			lcd_arrows(0,2);
		}
		if(menu_state==(MENU_MAX+MENU_ADCRESET)) {
			lcd_str("adc reset",0,0);
			lcd_num_from_right((DISPLAY_X/2),1,adc_reset);
			lcd_arrows(0,2);
		}
		if(menu_state==(MENU_MAX+MENU_INPUTSELECT)) {
			lcd_str("input sel.",0,0);
			if(input==0) {
				lcd_str("first",0,1);
			}
			else if(input==1) {
				lcd_str("second",0,1);
			}
			lcd_arrows(0,2);
		}
		if(menu_state==(MENU_MAX+MENU_ADCCHECK)) {
			lcd_str("adc check",0,0);
			if(adc_check) lcd_num_from_right((DISPLAY_X/2),1,adc_check);
			else lcd_str("disabled",0,1);
			lcd_arrows(0,2);
			lcd_str("0 to dsbl.",0,3);
		}
		if(menu_state==(MENU_MAX+MENU_UART)) {
			
			mode=MODE_UART;
			mode_update();
		}
		if(menu_state==(MENU_MAX+MENU_SPECTRUMXZOOM)) {
			lcd_str("sp. xzoom",0,0);
			lcd_num_from_right(DISPLAY_X/2-1,1,1<<spectrum_x_zoom);
			lcd_arrows(0,2);
		}
		if(menu_state==(MENU_MAX+MENU_SPECTRUMYZOOM)) {
			lcd_str("sp. yzoom",0,0);
			lcd_num_from_right(DISPLAY_X/2-1,1,1<<spectrum_y_zoom);
			lcd_arrows(0,2);
		}
		if(menu_state==(MENU_MAX+MENU_MODE)) {
			lcd_str("mode",0,0);
			if(mode==MODE_SIGNAL) lcd_str("signal",0,1);
			else if(mode==MODE_SPECTRUM) {
				lcd_str("spectrum",0,1);
				//if(mode==MODE_SPECTRUM||mode==MODE_DUAL) fft_maxfreq();
			}
			else if(mode==MODE_XY) lcd_str("xy",0,1);
			else if(mode==MODE_DUAL) {
				lcd_str("dual",0,1);
				//if(mode==MODE_SPECTRUM||mode==MODE_DUAL) fft_maxfreq();
			}
			lcd_arrows(0,2);
		}
		if(menu_state==MENU_EXIT) lcd_str("exit",0,1);
		if(menu_state==MENU_ADCPERIOD) lcd_str("adc period",0,1);
		if(menu_state==MENU_ADCSTEP) lcd_str("adc p step",0,1);
		if(menu_state==MENU_LCDSKIP) lcd_str("lcd skip",0,1);
		if(menu_state==MENU_ADCCHECK) lcd_str("adc check",0,1);
		if(menu_state==MENU_ADCERROR) lcd_str("adc error",0,1);
		if(menu_state==MENU_ADCRESET) lcd_str("adc reset",0,1);
		if(menu_state==MENU_SPECTRUMXZOOM) lcd_str("sp. xzoom",0,1);
		if(menu_state==MENU_SPECTRUMYZOOM) lcd_str("sp. yzoom",0,1);
		if(menu_state==MENU_ABOUT) lcd_str("about",0,1);
		if(menu_state==MENU_MODE) lcd_str("mode",0,1);
		if(menu_state==MENU_UART) lcd_str("pc mode",0,1);
		if(menu_state==MENU_INPUTSELECT) lcd_str("input sel.",0,1);*/
	}
}
