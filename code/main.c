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

#define top_state() (PINB&0x01)
#define right_state() (PINB&0x02)
#define left_state() (PINB&0x04)
#define up_state() (PINB&0x10)
#define down_state() (PINB&0x20)
#define pause_state() (PINB&0x08)

#define increment(a,min,max) a=(a==max)?min:a+1
#define decrement(a,min,max) a=(a==min)?max:a-1

#define incr_step(a,min,max,step) a=((a>(max-step))?min:a+step)
#define decr_step(a,min,max,step) a=((a<(min+step))?max:a-step)

#define fft_maxfreq() lcd_str("8px=",64,0);lcd_num_from_right(128,0,(F_CPU/(adc_period))>>4);lcd_str("hz",DISPLAY_X-11,1)
#define dfreq() lcd_str("dfreq/2=",0,0);lcd_num_from_right(63,1,F_CPU/adc_period/2)


//menu items
#define MENU_EXIT 0
#define MENU_ADCDELAY 1
#define MENU_ADCSTEP 2
#define MENU_MODE 3
#define MENU_LCDSKIP 4
#define MENU_ABOUT 5

#define MENU_MAX 5

//program modes
#define	MODE_MIN 1

#define MODE_SIGNAL 1
#define MODE_SPECTRUM 2
#define MODE_XY 3
#define MODE_DUAL 4
#define MODE_UART 5
#define MODE_UART_BUF 6
#define MODE_MAX 6

#define LCD_SKIP_MIN 1
#define LCD_SKIP_MAX 255

#define ADC_PERIOD_MIN 40
#define ADC_PERIOD_MAX 65535

#define ADC_STEP_MIN 1

#define adc_period OCR1B

uint16_t capture[ALL_N];
complex_t bfly_buff[ALL_N];		/* FFT buffer */
uint16_t output[ALL_N/2];

uint8_t current=0,
	mode=MODE_DUAL,
	top_state1=0,
	left_state1=0,
	right_state1=0,
	up_state1=0,
	down_state1=0,
	redraw_menu=0,
	adc_step=20,
	ovf=0,
	wait_ok=0,
	countdown=0;
int8_t menu_state=-1; //-1 disabled
				   //0 display exit 
				   //1...MENU_MAX - display option
				   //MENU_MAX+1...2*MENU_MAX - entered in
//counters
uint8_t m=1,s,u,u1,lcd_skip=1,lcd_skip_c=1;
uint16_t ymin,ymax,c,c1;
uint32_t wait=0,maxwait=0;

inline void buttons_update() {
	right_state1=right_state();
	left_state1=left_state();
	top_state1=top_state();
	up_state1=up_state();
	down_state1=down_state();
}

ISR(TIMER1_OVF_vect) {
	if(countdown) {
		if(ovf) {
			ovf--;
		}
	}
	else ovf++;
}

ISR(TIMER1_COMPB_vect) {
	TCNT1=0;
	ADCSRA|=1<<6;
}

ISR(TIMER1_COMPA_vect) {
	if(countdown&&!ovf) {
		wait_ok=1;
		TIMSK1&=~((1<<OCIE1A)|(1<<TOIE1));
	}
}


ISR(ADC_vect) {
	if((current<ALL_N)||mode==MODE_UART) {
		if(mode==MODE_UART) current=ALL_N-1;
		capture[current]=ADC>>1;
		if(mode==MODE_XY) {
			if(!(current%2)) ADMUX=0b01100001;
			else ADMUX=0b01100000;
		}
		if(mode!=MODE_UART) current++;
	}
}


ISR(TIMER0_OVF_vect) {
	while(!(pause_state())&&menu_state==-1) {
		if(mode==MODE_SPECTRUM||mode==MODE_DUAL) fft_maxfreq();
		if(mode==MODE_SIGNAL||mode==MODE_DUAL) dfreq();
		if(!top_state()&&top_state1) {
			menu_state=0;
			redraw_menu=1;
		}
		buttons_update();
		delay_us(1);
	}
	if(menu_state==-1) {
		if(!right_state()&&right_state1) {
			incr_step(adc_period,ADC_PERIOD_MIN,ADC_PERIOD_MAX,adc_step);
			if(mode==MODE_SPECTRUM||mode==MODE_DUAL) {
				fft_maxfreq();
				dfreq();
				maxwait=0;
				delay_ms(200);
			}
		}
		else if(!left_state()&&left_state1) {
			decr_step(adc_period,ADC_PERIOD_MIN,ADC_PERIOD_MAX,adc_step);
			if(mode==MODE_SPECTRUM||mode==MODE_DUAL) {
				fft_maxfreq();
				dfreq();
				maxwait=0;
				delay_ms(200);
			}
		}
		else if(!up_state()&&up_state1) {
			increment(adc_period,ADC_PERIOD_MIN,ADC_PERIOD_MAX);
			if(mode==MODE_SPECTRUM||mode==MODE_DUAL) {
				fft_maxfreq();
				dfreq();
				maxwait=0;
				delay_ms(200);
			}
		}
		else if(!down_state()&&down_state1) {
			decrement(adc_period,ADC_PERIOD_MIN,ADC_PERIOD_MAX);
			if(mode==MODE_SPECTRUM||mode==MODE_DUAL) {
				fft_maxfreq();
				dfreq();
				maxwait=0;
				delay_ms(200);
			}
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
			}
			else if(!left_state()&&left_state1) {
				decrement(mode,MODE_MIN,MODE_MAX);
				if(mode!=MODE_XY) ADMUX=0b01100000;
				redraw_menu=1;
			}
		}
		else if(menu_state==(MENU_MAX+MENU_ADCDELAY)) {
			if(!right_state()&&right_state1) {
				increment(adc_period,ADC_PERIOD_MIN,ADC_PERIOD_MAX);
				redraw_menu=1;
			}
			else if(!left_state()&&left_state1) {
				decrement(adc_period,ADC_PERIOD_MIN,ADC_PERIOD_MAX);
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
				maxwait=0;
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

inline uint16_t min(uint16_t a,uint16_t b) {
	return(a<b?a:b);
}

inline uint16_t max(uint16_t a,uint16_t b) {
	return(a>b?a:b);
}

int main() {
	// pins init
    DDRB&=~(0x3f);
    DDRA|=1<<7;
    PORTA&=~(1<<7);
    PORTB|=0x3f;
		uart_init();
    lcd_init();
    ADMUX=0b01100000;
    ADCSRA=0b11011101;
    SPCR=0x53;
    
    
    //button & adc interrupt init
		TCCR0B=0b101;
		TIMSK0=1<<TOIE0;
		TIMSK1=(1<<OCIE1B);
    TCNT0=0x00;
    TCNT1=0x00;
    adc_period=ADC_PERIOD_MIN;
    
    TCCR1B=0b001;
    sei();
    for(;;) {
		if(current>=(ALL_N-1)) {
			TIMSK0&=~(1<<TOIE0);
			TIMSK1&=~(1<<OCIE1B);
			ovf=0;
			TCNT1=0;
			TIMSK1|=1<<TOIE1;
			if(redraw_menu) {
				lcd_all(0);
				redraw_menu=0;
				if(menu_state>MENU_MAX) {
					if(menu_state==(MENU_MAX+MENU_ADCDELAY)) {
						lcd_str("adc delay",0,0);
						lcd_pixel_line_from_left(1,(adc_period-ADC_PERIOD_MIN)>>3);
						lcd_arrows(0,2);
					}
					if(menu_state==(MENU_MAX+MENU_ABOUT)) {
						lcd_str("digital",0,0);
						lcd_str("oscill.",10,1);
						lcd_str("v1.0",0,2);
						lcd_str("fft lib.",0,4);
						lcd_str("by chan",0,5);
						lcd_str("sergey",64,0);
						lcd_str("volodin",70,1);
						lcd_str("2011",80,2);
						lcd_str("i.179e.net",64,4);
					}
					if(menu_state==(MENU_MAX+MENU_LCDSKIP)) {
						lcd_str("lcd_skip",0,0);
						lcd_pixel_line_from_left(1,lcd_skip);
						lcd_arrows(0,2);
					}
					if(menu_state==(MENU_MAX+MENU_ADCSTEP)) {
						lcd_str("adc step",0,0);
						lcd_num_from_right(64,1,adc_step);
						lcd_arrows(0,2);
					}
					if(menu_state==(MENU_MAX+MENU_MODE)) {
						lcd_str("mode",0,0);
						if(mode==MODE_SIGNAL) {
							lcd_str("signal",0,1);
						}
						else if(mode==MODE_SPECTRUM) {
							lcd_str("spectrum",0,1);
							if(mode==MODE_SPECTRUM||mode==MODE_DUAL) fft_maxfreq();
						}
						else if(mode==MODE_XY) {
							lcd_str("xy",0,1);
						}
						else if(mode==MODE_DUAL) {
							lcd_str("dual",0,1);
							if(mode==MODE_SPECTRUM||mode==MODE_DUAL) fft_maxfreq();
						}
						else if(mode==MODE_UART) {
							lcd_str("uart",0,1);
						}
						else if(mode==MODE_UART_BUF) {
							lcd_str("uart buf",0,1);
						}
						lcd_arrows(0,2);
					}
				}
				else if(menu_state!=-1) {
					lcd_str("menu",0,0);
					if(menu_state==MENU_EXIT) lcd_str("exit",0,1);
					if(menu_state==MENU_ADCDELAY) lcd_str("adc delay",0,1);
					if(menu_state==MENU_ADCSTEP) lcd_str("adc d step",0,1);
					if(menu_state==MENU_LCDSKIP) lcd_str("lcd skip",0,1);
					if(menu_state==MENU_ABOUT) lcd_str("about",0,1);
					if(menu_state==MENU_MODE) lcd_str("mode",0,1);
					lcd_arrows(0,2);
				}
			}
			if(menu_state==-1) {
				if((mode==MODE_SPECTRUM)||(mode==MODE_DUAL)) {
						fft_input(capture, bfly_buff);
						fft_execute(bfly_buff);
						fft_output(bfly_buff, output);
						for (m=0;m<ALL_N/2;m++) {
							s=output[m]>>9;
							if(s>63) s=63;
							if(s<0) s=0;
							for(u=0;u<7;u++) lcd_block(ALL_N/2+m,u,0);
							if(!(m%8)) {
								lcd_goto_xblock(ALL_N/2+m);
								lcd_goto_yblock(2);
								lcd_databits(SEND_DATA,0x1);
							}
							lcd_line_from_bottom(ALL_N/2+m,s);
						}
				}
				if((mode==MODE_SIGNAL)||(mode==MODE_DUAL)) {
					if(lcd_skip_c>=lcd_skip) {
						lcd_skip_c=1;
						for(m=0;m<ALL_N;m++) {
							if(mode==MODE_DUAL)  {
								c=63-(capture[m]>>9);
								
								//prev
								if(m>1) {
									c1=(63-(capture[m-2]>>9));
									if(c1<c) ymin=(c1+c)>>1;
									if(c1>c) ymax=(c1+c)>>1;
								}
								else {
									ymin=c;
									ymax=c;
								}
								
								//next
								if(m<(ALL_N-2)) {
									c1=63-(capture[m+2]>>9);
									if(c1<c) ymin=min(ymin,(c+c1)>>1);
									if(c1>c) ymax=max(ymax,(c1+c)>>1);
								}
								for(s=0;s<8;s++) lcd_block(m/2,s,0);
								lcd_constx_line(m/2,ymin,ymax);
								m++;
							}
							else {
								c=63-(capture[m]>>9);
								
								//prev
								if(m) {
									c1=(63-(capture[m-1]>>9));
									if(c1<c) ymin=(c1+c)>>1;
									if(c1>c) ymax=(c1+c)>>1;
								}
								else {
									ymin=c;
									ymax=c;
								}
								
								//next
								if(m<(ALL_N-1)) {
									c1=63-(capture[m+1]>>9);
									if(c1<c) ymin=min(ymin,(c+c1)>>1);
									if(c1>c) ymax=max(ymax,(c1+c)>>1);
								}
								for(s=0;s<8;s++) lcd_block(m,s,0);
								lcd_constx_line(m,ymin,ymax);
							}
						}
					}
					else lcd_skip_c++;
				}
				else if(mode==MODE_XY) {
					if(lcd_skip_c>=lcd_skip) {
						lcd_skip_c=1;
						for(s=64;s<128;s++) {
							for(m=0;m<8;m++) lcd_block(s,m,0);
						}
					}
					else lcd_skip_c++;
					
					for(s=0;s<128;s+=2) {
						lcd_pixel_share(ALL_N/2+(capture[s]>>9),63-(capture[s+1]>>9));
					}
				}
			}
			else if(mode==MODE_UART_BUF) {
				m=0;u=0;
				while(m<ALL_N) {
					s=capture[m++]>>7;
					if(s>=256) uart_communicate(254);
					else uart_communicate(s);
					if(u_c==U_M) u++;
					if(u>=10) m=ALL_N;
				}
			}
			else if(mode==MODE_UART) {
				s=capture[ALL_N-1]>>7;
				if(s>=256) uart_communicate(254);
				else uart_communicate(s);
			}
			
			TIMSK1&=~(1<<TOIE1);
	
			//wait=(ovf*65536+TCNT1)%adc_period;
			
			wait=(ovf*65536+TCNT1);
			//if(maxwait<wait&&menu_state==-1) maxwait=wait+(adc_period-wait%adc_period);
			wait=adc_period-wait%adc_period;
			if(wait) {
				TCNT1=0;
				OCR1A=wait%65536;
				ovf=(wait-OCR1A)/65536;
				wait_ok=0;
				countdown=1;
				PORTA|=1<<7;
				lcd_num_from_right(60,4,ovf);
				lcd_num_from_right(60,5,OCR1A);
				lcd_num_from_right(127,5,wait);
				TIMSK1|=(1<<TOIE1)|(1<<OCIE1A);
				while((TIMSK1&(1<<OCIE1A))) asm("nop");
				PORTA&=~(1<<7);
			}
			TCNT1=adc_period;
			TIMSK1|=1<<OCIE1B;
			TIMSK0|=1<<TOIE0;
			current=0;
		}
		asm("nop");
	}
}
