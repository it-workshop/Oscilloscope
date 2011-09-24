#ifndef SETTINGS_H
#define SETTINGS_H

//settings
#define ALL_N 128

#define V_MAX (65536/2-1)

#define LCD_SKIP_MIN 1
#define LCD_SKIP_MAX 4096
#define LCD_SKIP_STEP 1

#define ADC_PERIOD_MIN 100
#define ADC_PERIOD_MAX 65535
#define ADC_PERIOD_SCALE 4

#define ADC_STEP 10

#define ADC_ERROR_MAX 32767
#define ADC_ERROR_STEP 326

#define ADC_RESET_DEFAULT (3*ALL_N)
#define ADC_RESET_STEP ALL_N
#define ADC_RESET_MIN ADC_RESET_STEP
#define ADC_RESET_INF (10*ALL_N)

#define SPECTRUM_ZOOM_MIN 0
#define SPECTRUM_ZOOM_MAX_Y 6
#define SPECTRUM_ZOOM_MAX_X 5

#define adc_period OCR1B

#define UART_RESP_GET 1
#define UART_RESP_OTHER 0
#define UART_ERROR 2

//program modes
#define	MODE_MIN 1

#define MODE_SIGNAL 1
#define MODE_SPECTRUM 2
#define MODE_XY 3
#define MODE_DUAL 4
#define MODE_UART 5
#define MODE_UART_BUF 6

#define MODE_MAX 4

#define increment(a,min,max) a=(a==max)?min:a+1
#define decrement(a,min,max) a=(a==min)?max:a-1

#define min(x,y) (x<y?x:y)
#define max(x,y) (x>y?x:y)
#define abs(x) (x<0?-x:x)
#define absdiff(x,y) (x>y?(x-y):(y-x))

#define incr_step(a,min,max,step) a=((a>(max-step))?min:a+step)
#define decr_step(a,min,max,step) a=((a<(min+step))?max:a-step)

#define adc_request() ADCSRA|=1<<6
#define adc_timer_play() TIMSK1|=(1<<OCIE1B);TCCR1B=0b001
#define adc_timer_pause() TIMSK1&=~(1<<OCIE1B);TCCR1B=0;current=0
#define buttons_timer_play() TIMSK0|=(1<<TOIE0);TCCR0B=0b101
#define buttons_timer_pause() TIMSK0&=~(1<<TOIE0);TCCR0B=0

#define adc_freq_normal() ADCSRA=0b10011101
#define adc_freq_fast() ADCSRA=0b10011010
#define adc_first()  ADMUX=0b01100000
#define adc_second() ADMUX=0b01100001

void mode_update();
inline void return_control();

#endif
