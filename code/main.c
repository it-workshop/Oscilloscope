#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <math.h>
#include "uart.c"		/* Defs for using Software UART module (Debugging via AVRSP-COM) */
#include "ffft.h"		/* Defs for using Fixed-point FFT module */
#include "lcd.c"

#define lcd_arrows(x,y) lcd_str("< >",x,y)

#define top_state() (PINA&(1<<2))
#define right_state() (PINA&(1<<3))
#define left_state() (PINA&(1<<4))
#define up_state() (PINA&(1<<6))
#define down_state() (PINA&(1<<7))
#define pause_state() (PINA&(1<<5))

#define increment(a,min,max) a=(a==max)?min:a+1
#define decrement(a,min,max) a=(a==min)?max:a-1

#define min(x,y) (x<y?x:y)
#define max(x,y) (x>y?x:y)
#define abs(x) (x<0?-x:x)

#define incr_step(a,min,max,step) a=((a>(max-step))?min:a+step)
#define decr_step(a,min,max,step) a=((a<(min+step))?max:a-step)

#define fft_maxfreq() lcd_str("8px=",(DISPLAY_X/2),0);lcd_num_from_right(DISPLAY_X,0,(F_CPU/(adc_period*(1<<spectrum_x_zoom)))>>4);lcd_str("hz",DISPLAY_X-11,1)
#define dfreq() lcd_str("dfreq=",0,0);lcd_num_from_right(63,1,F_CPU/adc_period)
#define osd_delay() delay_ms(200)


//menu items
#define MENU_EXIT 0
#define MENU_MODE 1
#define MENU_ADCPERIOD 2
#define MENU_ADCCHECK 3
#define MENU_ADCERROR 4
#define MENU_ADCSTEP 5
#define MENU_LCDSKIP 6
#define MENU_SPECTRUMXZOOM 7
#define MENU_SPECTRUMYZOOM 8
#define MENU_ABOUT 9

#define MENU_MAX 9

//program modes
#define	MODE_MIN 1

#define MODE_SIGNAL 1
#define MODE_SPECTRUM 2
#define MODE_XY 3
#define MODE_DUAL 4
#define MODE_UART 5
#define MODE_UART_BUF 6

#define MODE_MAX 5

//settings
#define LCD_SKIP_MIN 1
#define LCD_SKIP_MAX 4096

#define ADC_PERIOD_MIN 80
#define ADC_PERIOD_MAX 65535

#define ADC_STEP_MIN 1

#define ADC_ERROR_STEP 100
#define ADC_ERROR_MAX 65535

#define SPECTRUM_ZOOM_MIN 0
#define SPECTRUM_ZOOM_MAX_Y 6
#define SPECTRUM_ZOOM_MAX_X 5

#define adc_period OCR1B

#define UART_RESP_GET 1
#define UART_RESP_OTHER 0

#define adc_request() ADCSRA|=1<<6
#define adc_timer_play() TIMSK1|=(1<<OCIE1B);TCCR1B=0b001
#define adc_timer_pause() TIMSK1&=~(1<<OCIE1B);TCCR1B=0
#define buttons_timer_play() TIMSK0|=(1<<TOIE0);TCCR0B=0b101
#define buttons_timer_pause() TIMSK0&=~(1<<TOIE0);TCCR0B=0

uint16_t capture[ALL_N];
complex_t bfly_buff[ALL_N];		/* FFT buffer */
uint16_t output[ALL_N/2];

//state variables
uint8_t current=0,
	mode=MODE_SIGNAL,
	top_state1=0,
	left_state1=0,
	right_state1=0,
	up_state1=0,
	down_state1=0,
	pause_state1=0,
	redraw_menu=0,
	adc_step=100,
	array_filled=0,
	error_storage=0,
	spectrum_x_zoom=0,
	spectrum_y_zoom=0,
	running=1;
int8_t menu_state=-1; //-1 disabled
	//0 display exit 
	//1...MENU_MAX - display option
	//MENU_MAX+1...2*MENU_MAX - entered in
uint16_t adc_error=15000,
	adc_check=10,
	adc_reset=10*ALL_N,
	lcd_skip=1;

//counters
uint8_t m=1,s,u;
uint16_t adc_reset_c=0,lcd_skip_c=1;

//temp
uint16_t ymin,ymax,c,c1;

inline void buttons_update() {
	right_state1=right_state();
	left_state1=left_state();
	top_state1=top_state();
	up_state1=up_state();
	down_state1=down_state();
	pause_state1=pause_state();
}

void osd() {
	if((mode==MODE_DUAL)||(mode==MODE_SPECTRUM)) {
		fft_maxfreq();
	}
	if((mode==MODE_SIGNAL)||(mode==MODE_DUAL)||(mode==MODE_XY)) {
		dfreq();
	}
}

void mode_update() {
	cli();
	redraw_menu=0;
	if((mode==MODE_UART_BUF)||(mode==MODE_UART)) {
		uart_init();
		lcd_all(0);
		lcd_str("uart mode",0,0);
		buttons_timer_pause();
	}
	else {
		uart_close();
	}
	if(mode==MODE_UART) {
		adc_timer_pause();
		ADCSRA&=~(1<<ADIE);
	}
	else {
		array_filled=0;
		running=1;
		ADCSRA|=(1<<ADIE);
		if(mode!=MODE_UART_BUF) {
			buttons_timer_play();
		}
		adc_timer_play();
	}
	current=0;
	sei();
}

uint8_t uart_action() {
	static uint8_t c;
	c=uart_getc();
	if(c=='g') {
		return(UART_RESP_GET);
	}
	else if(c=='c') {
		menu_state=-1;
		mode=MODE_SIGNAL;
		mode_update();
	}
	else if(c=='b') {
		adc_period=uart_getc()|(uart_getc()<<8);
		mode=MODE_UART_BUF;
		mode_update();
	}
	else if(c=='n') {
		mode=MODE_UART;
		mode_update();
	}
	return(UART_RESP_OTHER);
}

inline uint8_t todisplay(uint16_t a) {
	if(menu_state==-1) return(DISPLAY_Y-1-(a>>9));
	else return(DISPLAY_Y-1-(a>>10));
}

void draw_menu() {
	if(menu_state>MENU_MAX) {
		if(menu_state==(MENU_MAX+MENU_ADCPERIOD)) {
			lcd_str("adc period",0,0);
			lcd_pixel_line_from_left(1,(adc_period-ADC_PERIOD_MIN)>>3);
			lcd_num_from_right(127,2,adc_period);
			lcd_arrows(0,2);
		}
		if(menu_state==(MENU_MAX+MENU_ABOUT)) {
			lcd_str("digital",0,0);
			lcd_str("oscill.",10,1);
			lcd_str("v1.0",0,2);
			lcd_str("fft lib.",0,4);
			lcd_str("by chan",0,5);
			lcd_str("sergey",(DISPLAY_X/2),0);
			lcd_str("volodin",70,1);
			lcd_str("2011",80,2);
			lcd_str("i.179e.net",(DISPLAY_X/2),4);
		}
		if(menu_state==(MENU_MAX+MENU_LCDSKIP)) {
			lcd_str("lcd skip",0,0);
			lcd_pixel_line_from_left(1,lcd_skip>>1);
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
		if(menu_state==(MENU_MAX+MENU_ADCCHECK)) {
			lcd_str("adc check",0,0);
			if(adc_check) lcd_num_from_right((DISPLAY_X/2),1,adc_check);
			else lcd_str("disabled",0,1);
			lcd_arrows(0,2);
			lcd_str("0 to dsbl.",0,3);
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
				if(mode==MODE_SPECTRUM||mode==MODE_DUAL) fft_maxfreq();
			}
			else if(mode==MODE_XY) lcd_str("xy",0,1);
			else if(mode==MODE_DUAL) {
				lcd_str("dual",0,1);
				if(mode==MODE_SPECTRUM||mode==MODE_DUAL) fft_maxfreq();
			}
			else if(mode==MODE_UART) lcd_str("uart",0,1);
			else if(mode==MODE_UART_BUF) {
				lcd_str("uart buf",0,1);
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
		if(menu_state==MENU_SPECTRUMXZOOM) lcd_str("sp. xzoom",0,1);
		if(menu_state==MENU_SPECTRUMYZOOM) lcd_str("sp. yzoom",0,1);
		if(menu_state==MENU_ABOUT) lcd_str("about",0,1);
		if(menu_state==MENU_MODE) lcd_str("mode",0,1);
		lcd_arrows(0,2);
	}
}

void draw_signal() {
	for(m=0;m<ALL_N;m++) {
		c=todisplay(capture[m]);
		if(mode==MODE_DUAL)  {
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
			
			u=m/2;
			m++;
		}
		else {
			
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
			u=m;
		}
		if(menu_state==-1) {
			for(s=0;s<8;s++) lcd_block(u,s,0);
		}
		else {
			for(s=4;s<8;s++) lcd_block(u,s,0);
		}
		lcd_constx_line(u,ymin,ymax);
	}
}

ISR(TIMER1_COMPB_vect) {
	TCNT1=0;
	adc_request();
}

ISR(ADC_vect) {
	if((running&&(current<ALL_N))) {
		if(mode==MODE_SIGNAL||mode==MODE_SPECTRUM||mode==MODE_DUAL||mode==MODE_UART_BUF) {
			if(adc_reset_c>=adc_reset) {
				adc_reset_c=0;
				array_filled=0;
				current=0;
				return;
			}
			if(array_filled&&(abs(ADC-capture[current])>adc_error)&&current<adc_check) {
				adc_reset_c++;
				current=0;
				return;
			}
		}
		capture[current]=ADC>>1;
		if(mode==MODE_XY) {
			if(!(current%2)) {
				ADMUX=0b01100001;
				adc_request();
			}
			else ADMUX=0b01100000;
		}
		current++;
	}
}

ISR(TIMER0_OVF_vect) {
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
				incr_step(adc_error,ADC_ERROR_STEP,ADC_ERROR_MAX,ADC_ERROR_STEP);
				redraw_menu=1;
			}
			else if(!left_state()&&left_state1) {
				decr_step(adc_error,ADC_ERROR_STEP,ADC_ERROR_MAX,ADC_ERROR_STEP);
				redraw_menu=1;
			}
		}
		else if(menu_state==(MENU_MAX+MENU_ADCSTEP)) {
			if(!right_state()&&right_state1) {
				increment(adc_step,ADC_STEP_MIN,ADC_PERIOD_MAX);
				redraw_menu=1;
			}
			else if(!left_state()&&left_state1) {
				decrement(adc_step,ADC_STEP_MIN,ADC_PERIOD_MAX);
				redraw_menu=1;
			}
		}
		else if(menu_state==(MENU_MAX+MENU_LCDSKIP)) {
			if(!right_state()&&right_state1) {
				increment(lcd_skip,LCD_SKIP_MIN,LCD_SKIP_MAX);
				redraw_menu=1;
			}
			else if(!left_state()&&left_state1) {
				decrement(lcd_skip,LCD_SKIP_MIN,LCD_SKIP_MAX);
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

int main() {
	lcd_init();
	lcd_str("booting",0,0);

	// pins init
	DDRA=0x00;
	PORTA=252;
	
	//adc init
	ADMUX=0b01100000;
	ADCSRA=0b11011101;

	//button & adc interrupt init
	TCCR0B=0b101;
	TCNT0=0x00;
	TCNT1=0x00;
	adc_period=ADC_PERIOD_MIN;
	delay_ms(100);
	lcd_all(0);
	mode_update();
	sei();
	for(;;) {
		if(mode==MODE_UART) {
			if(uart_action()==UART_RESP_GET) {
				adc_request();
				while(!(ADCSRA&(1<<ADIF))) {
					asm("nop");
				}
				uart_putc(ADCL);
				uart_putc(ADCH);
			}
		}
		else {
			if(redraw_menu) {
				lcd_all(0);
				if(running) redraw_menu=0;
				draw_menu();
			}
			if((current>=(ALL_N-1))||((!running)&&redraw_menu)) {
				if(!running) redraw_menu=0;
				if(!array_filled) array_filled=1;
				adc_reset_c=0;
				if((mode==MODE_SPECTRUM)||(mode==MODE_DUAL)) {
						fft_input(capture, bfly_buff);
						fft_execute(bfly_buff);
						fft_output(bfly_buff, output);
						for (m=0;m<ALL_N/2;m++) {
							s=output[m]>>(9-spectrum_y_zoom);
							if(s>63) s=63;
							if(s<0) s=0;
							for(u=0;u<7;u++) lcd_block(ALL_N/2+m,u,0);
							if(!(m%8)) {
								lcd_goto_xblock(ALL_N/2+m);
								lcd_goto_yblock(2);
								lcd_databits(SEND_DATA,0x1);
							}
							c=(DISPLAY_X/2)+(m<<spectrum_x_zoom);
							if(c<DISPLAY_X) lcd_line_from_bottom(c,s);
						}
				}
				if((mode==MODE_SIGNAL)||(mode==MODE_DUAL)) {
					if(lcd_skip_c>=lcd_skip) {
						lcd_skip_c=1;
						draw_signal();
					}
					else lcd_skip_c++;
				}
				else if(mode==MODE_XY) {
					if(lcd_skip_c>=lcd_skip) {
						lcd_skip_c=1;
						for(s=(DISPLAY_X/2);s<DISPLAY_X;s++) {
							for(m=0;m<8;m++) lcd_block(s,m,0);
						}
					}
					else lcd_skip_c++;
					
					for(s=0;s<DISPLAY_X;s+=2) {
						lcd_pixel_share(ALL_N/2+(capture[s]>>9),63-(capture[s+1]>>9));
					}
				}
				else if(mode==MODE_UART_BUF) {
					adc_timer_pause();
					for(m=0;m<ALL_N;m++) {
						if(uart_action()==UART_RESP_GET) {
							uart_putc(capture[m]^(0xff<<8));
							uart_putc(capture[m]>>8);
						}
						else {
							m=ALL_N;
						}
					}
					adc_timer_play();
					uart_flush();
				}
				current=0;
				if(!running) osd();
			}
		}
	}
}
