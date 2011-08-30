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
	adc_step,
	array_filled,
	error_storage,
	spectrum_x_zoom,
	spectrum_y_zoom,
	running;
extern int8_t menu_state;
extern uint16_t adc_error,
	adc_check,
	adc_reset,
	lcd_skip;


void draw_menu() {
	if(menu_state>MENU_MAX) {
		if(menu_state==(MENU_MAX+MENU_ADCPERIOD)) {
			lcd_str("adc period",0,0);
			lcd_pixel_line_from_left(1,(adc_period-ADC_PERIOD_MIN)>>5);
			lcd_num_from_right((DISPLAY_X)/2,3,adc_period);
			lcd_arrows(0,2);
		}
		if(menu_state==(MENU_MAX+MENU_ABOUT)) {
			welcome();
			_delay_ms(1000);
			menu_state-=MENU_MAX;
			redraw_menu=1;
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
		if(menu_state==(MENU_MAX+MENU_ADCCHECK)) {
			lcd_str("adc check",0,0);
			if(adc_check) lcd_num_from_right((DISPLAY_X/2),1,adc_check);
			else lcd_str("disabled",0,1);
			lcd_arrows(0,2);
			lcd_str("0 to dsbl.",0,3);
		}
		if(menu_state==(MENU_MAX+MENU_UART)) {
			lcd_str("pc mode",0,0);
			lcd_str("you can return\ncontrol from pc or\nreboot the device.",0,2);
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
	}
	else if(menu_state!=-1) {
		lcd_str("menu",0,0);
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
		lcd_arrows(0,2);
	}
}
