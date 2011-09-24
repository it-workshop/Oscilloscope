#include <avr/pgmspace.h>
#include <stdio.h>
#include "draw.h"
#include "lcd.h"
#include "menu.h"
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
	running;
extern uint16_t adc_error,
	adc_check,
	adc_reset,
	lcd_skip;

inline uint8_t todisplay(uint16_t a) {
	if(menu_state==MENU_NONE) return(DISPLAY_Y-1-(a>>9));
	else return(DISPLAY_Y-1-(a>>10));
}

void draw_signal(uint8_t draw_what,uint8_t draw_type) {
	for(m=0;m<ALL_N;m++) {
		c=todisplay(capture[m]);
		ymin=c;
		ymax=c;
		if(draw_what==DRAW_SIGNAL_DUAL) {
			if(draw_type==DRAW_LINES) {
				//prev
				if(m>1) {
					c1=todisplay(capture[m-2]);
					if(c1<c) ymin=(c1+c)>>1;
					if(c1>c) ymax=(c1+c)>>1;
				}
				else {
					ymin=c;
					ymax=c;
				}
				
				//next
				if(m<(ALL_N-2)) {
					c1=todisplay(capture[m+2]);
					if(c1<c) ymin=min(ymin,(c+c1)>>1);
					if(c1>c) ymax=max(ymax,(c1+c)>>1);
				}
			}
			u=m/2;
			m++;
		}
		else {
			if(draw_type==DRAW_LINES) {
				//prev
				if(m) {
					c1=todisplay(capture[m-1]);
					if(c1<c) ymin=(c1+c)>>1;
					if(c1>c) ymax=(c1+c)>>1;
				}
				else {
					ymin=c;
					ymax=c;
				}
				
				//next
				if(m<(ALL_N-1)) {
					c1=todisplay(capture[m+1]);
					if(c1<c) ymin=min(ymin,(c+c1)>>1);
					if(c1>c) ymax=max(ymax,(c1+c)>>1);
				}
			}
			u=m;
		}
		if(menu_state==MENU_NONE) {
			for(s=0;s<8;s++) lcd_block(u,s,0);
		}
		else {
			for(s=4;s<8;s++) lcd_block(u,s,0);
		}
		if(draw_type==DRAW_LINES) {
			lcd_constx_line(u,ymin,ymax);
		}
		else if(draw_type==DRAW_DOTS) {
			lcd_pixel(u,c);
		}
	}
}


void osd() {
	if((mode==MODE_DUAL)||(mode==MODE_SPECTRUM)) {
		fft_maxfreq();
	}
	if((mode==MODE_SIGNAL)||(mode==MODE_DUAL)||(mode==MODE_XY)) {
		dfreq();
	}
}

void welcome() {
	lcd_str("digital oscilloscope\n  v1.0",0,0);
	lcd_str("volodin sergey\ni.179e.net",0,3);
	lcd_str("fft lib. by chan",0,6);
}

inline void dfreq_only(uint8_t x,uint8_t y) {
	lcd_num_from_right(x,y,F_CPU/adc_period);
}

inline void fft_maxfreq() {
	lcd_str("8px=",(DISPLAY_X/2)-1,0);
	lcd_num_from_right(DISPLAY_X-1,0,
	 (F_CPU/(adc_period*(1<<spectrum_x_zoom)))>>4);
	lcd_str("hz",DISPLAY_X-13,1);
}
inline void dfreq() {
	lcd_str("dfreq=",0,0);
	dfreq_only((DISPLAY_X/2)-1,1);
}
