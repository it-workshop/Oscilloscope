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
#include "menu.h"

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
       clear_screen,
       array_filled,
       error_storage,
       spectrum_x_zoom,
       spectrum_y_zoom,
       running,
       menu_state,
       trigger_enabled,
       input;
extern uint16_t adc_error,
       adc_check,
       adc_reset,
       lcd_skip;

extern uint8_t m, s, u;


//temp
extern uint16_t ymin, ymax, c, c1;

void mode_update()
{
    cli();
    if((mode == MODE_UART_BUF) || (mode == MODE_UART))
    {
        redraw_menu = 1;
        buttons_timer_pause();
    }
    else
    {
        buttons_timer_play();
        redraw_menu = 1;
        clear_screen = 1;
    }
    if(mode == MODE_XY)
    {
        adc_freq_fast();
    }
    else
    {
        if(input == 0)
        {
            adc_first();
        }
        else if(input == 1)
        {
            adc_second();
        }
        //adc_freq_normal();
        adc_freq_fast();
    }
    if((mode == MODE_UART) || (mode == MODE_VOLTAGE))
    {
        adc_timer_pause();
        adc_interrupt_pause();
        if(mode == MODE_VOLTAGE)
        {
            buttons_timer_play();
        }
    }
    else
    {
        running = 1;
        adc_interrupt_play();
        if(mode != MODE_UART_BUF)
        {
            buttons_timer_play();
        }
        if(running || mode == MODE_UART_BUF)
        {
            adc_timer_play();
        }
        else
        {
            adc_timer_pause();
        }
    }
    current = 0;
    trigger_enabled = (mode != MODE_XY && mode != MODE_SPECTRUM);
    sei();
}

void return_control()
{
    mode = MODE_DUAL;
    menu_state = MENU_NONE;
    m = 0;
    u = 0;
    c = 0;
    array_filled = 0;
    running = 1;
    lcd_all(0);
    adc_period = ADC_PERIOD_MIN;
    mode_update();
}
