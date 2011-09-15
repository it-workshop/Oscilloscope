#define MAIN_C
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <math.h>
#include "settings.h"
#include "uart.h"
#include "menu.h"
#include "lcd.h"
#include "draw.h"
#include "ffft.h"
#include "buttons.h"

uint16_t capture[ALL_N];
complex_t bfly_buff[ALL_N];		/* FFT buffer */
uint16_t output[ALL_N/2];


int8_t menu_state=-1; //-1 disabled
	//0 display exit 
	//1...MENU_MAX - display option
	//MENU_MAX+1...2*MENU_MAX - entered in

//state variables
uint8_t current=0,
	mode=MODE_DUAL,
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
	running=1,
	input=0;
uint16_t adc_error=1,
	adc_check=2,
	adc_reset=ADC_RESET_DEFAULT,
	lcd_skip=1;

//counters
uint8_t m=1,s,u;
uint16_t adc_reset_c=0,lcd_skip_c=1;


//temp
uint16_t ymin,ymax,c,c1;

ISR(TIMER1_COMPB_vect) {
	TCNT1=0;
	adc_request();
}

ISR(TIMER0_OVF_vect) {
	buttons_process();
}

ISR(ADC_vect) {
	if(((running||mode==MODE_UART_BUF)&&(current<ALL_N))) {
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
				adc_second();
				adc_request();
			}
			else adc_first();
		}
		current++;
	}
}


int main() {
	lcd_init();
	uart_init();
	welcome();

	SPCR=(1<<6)|0b11|(1<<4);

	// pins init
	DDRA=0x00;
	PORTA=252;
	
	//adc init
	adc_first();
	adc_freq_normal();
	
	//button & adc interrupt init
	TCCR0B=0b101;
	TCNT0=0x00;
	TCNT1=0x00;
	adc_period=ADC_PERIOD_MIN;
	
	_delay_ms(1000);
	lcd_all(0);
	mode_update();
	sei();
	for(;;) {
		if(mode==MODE_UART) {
			adc_request();
			while(!(ADCSRA&(1<<ADIF))) {
				asm("nop");
			}
			capture[0]=ADC;
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
				if(mode==MODE_SPECTRUM||mode==MODE_SIGNAL||mode==MODE_DUAL) {
					if(lcd_skip_c>=lcd_skip) {
						lcd_skip_c=1;
						if((mode==MODE_SPECTRUM)||(mode==MODE_DUAL)) {
							fft_input((int16_t*)capture, bfly_buff);
							fft_execute(bfly_buff);
							fft_output(bfly_buff, output);
							for (m=0;m<ALL_N/2;m++) {
								s=output[m]>>(9-spectrum_y_zoom);
								if(s>DISPLAY_Y-1) s=DISPLAY_Y-1;
								if(s<0) s=0;
								for(u=0;u<=7;u++) {
									lcd_block(ALL_N/2+m,u,0);
								}
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
							draw_signal();
						}
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
					while(current) {
						_delay_ms(1);
					}
				}
				current=0;
				if(!running&&menu_state==-1) osd();
			}
		}
		_delay_ms(1);
	}
}
