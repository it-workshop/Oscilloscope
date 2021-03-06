#include <avr/pgmspace.h>
#include "util/delay.h"
#include <stdio.h>
#include "draw.h"
#include "buttons.h"
#include "lcd.h"
#include "menu.h"
#include "settings.h"

extern uint16_t capture[ALL_N];

extern uint8_t current,
       mode,
       top_state1,
       left_state1,
       right_state1,
       up_state1,
       down_state1,
       running1,
       redraw_menu,
       array_filled,
       error_storage,
       spectrum_x_zoom,
       spectrum_y_zoom,
       running,
       input,
       vzoom,
       tzoom,
       signal_type,
       m, u, s;
extern int8_t menu_state, vpos;
extern uint16_t adc_error,
       adc_check,
       adc_reset,
       lcd_skip;

void buttons_process()
{
    running = isrunning();

    if(!running1 && running)
    {
        adc_timer_play();
    }
    if(running1 && !running)
    {
        adc_timer_pause();
    }

    if(menu_state == MENU_NONE)
    {
        if(right_pressed())
        {
            decr_step(adc_period, ADC_PERIOD_MIN, ADC_PERIOD_MAX,
                      (adc_period >> ADC_PERIOD_SCALE));
            osd();
            osd_delay();
        }
        else if(left_pressed())
        {
            incr_step(adc_period, ADC_PERIOD_MIN, ADC_PERIOD_MAX,
                      (adc_period >> ADC_PERIOD_SCALE));
            osd();
            osd_delay();
        }
        else if(up_pressed())
        {
            decrement(adc_period, ADC_PERIOD_MIN, ADC_PERIOD_MAX);
            osd();
            osd_delay();
        }
        else if(down_pressed())
        {
            increment(adc_period, ADC_PERIOD_MIN, ADC_PERIOD_MAX);
            osd();
            osd_delay();
        }
        else if(top_pressed())
        {
            menu_state = MENU_EXIT;
            lcd_all(0);
            redraw_menu = 1;
        }
    }
    else if(menu_attoplevel())
    {
        if(top_pressed() || right_pressed() || left_pressed()
                || up_pressed() || down_pressed()) redraw_menu = 1;
        if(top_pressed())
        {
            if(menu_state == MENU_EXIT)
            {
                menu_state = MENU_NONE;
                top_state1 = 0;
                redraw_menu = 1;
            }
            else
            {
                menu_state |= 1;
            }
        }
        else menu_top_change();
    }
    else
    {
        if(up_pressed())
        {
            if(menu_state != MENU_TRIGGER_VALUES_EDIT)
            {
                increment(menu_state, menu_topstate() + 1, menu_second_max());
            }
            else
            {
                incr_step(capture[current], 0, V_MAX, 655);
                redraw_menu = 1;
            }
            redraw_menu = 1;
        }
        else if(down_pressed())
        {
            if(menu_state != MENU_TRIGGER_VALUES_EDIT)
            {
                decrement(menu_state, menu_topstate() + 1, menu_second_max());
            }
            else
            {
                decr_step(capture[current], 0, V_MAX, 655);
                redraw_menu = 1;
            }
            redraw_menu = 1;
        }
        else if(top_pressed())
        {
            if(menu_state == MENU_TRIGGER_VALUES_EDIT)
            {
                menu_state = MENU_TRIGGER_VALUES;
                current = 0;
                array_filled = 1;
                adc_timer_play();
            }
            else
            {
                menu_state = menu_topstate();
            }
            redraw_menu = 1;
        }

        else if(menu_state == MENU_MODE_MODES)
        {
            if(right_pressed())
            {
                increment(mode, MODE_MIN, MODE_MAX);
                redraw_menu = 1;
                mode_update();
            }
            else if(left_pressed())
            {
                decrement(mode, MODE_MIN, MODE_MAX);
                redraw_menu = 1;
                mode_update();
            }
        }
        else if(menu_state == MENU_MODE_UART)
        {
            if(right_pressed())
            {
                mode = MODE_UART;
                redraw_menu = 1;
            }
        }

        else if(menu_state == MENU_ADC_FREQ)
        {
            if(right_pressed())
            {
                decr_step(adc_period, ADC_PERIOD_MIN, ADC_PERIOD_MAX,
                          (adc_period >> ADC_PERIOD_SCALE));
                redraw_menu = 1;
            }
            else if(left_pressed())
            {
                incr_step(adc_period, ADC_PERIOD_MIN, ADC_PERIOD_MAX,
                          (adc_period >> ADC_PERIOD_SCALE));
                redraw_menu = 1;
            }
        }
        else if(menu_state == MENU_ADC_INPUT)
        {
            if(right_pressed() || left_pressed())
            {
                input = input ? 0 : 1;
                redraw_menu = 1;
                mode_update();
            }
        }

        else if(menu_state == MENU_TRIGGER_CHECK)
        {
            if(right_pressed())
            {
                increment(adc_check, 0, ALL_N - 1);
                redraw_menu = 1;
            }
            else if(left_pressed())
            {
                decrement(adc_check, 0, ALL_N - 1);
                redraw_menu = 1;
            }
        }
        else if(menu_state == MENU_TRIGGER_ERROR)
        {
            if(right_pressed())
            {
                incr_step(adc_error, 0, ADC_ERROR_MAX, ADC_ERROR_STEP);
                redraw_menu = 1;
            }
            else if(left_pressed())
            {
                decr_step(adc_error, 0, ADC_ERROR_MAX, ADC_ERROR_STEP);
                redraw_menu = 1;
            }
        }
        else if(menu_state == MENU_TRIGGER_RESET)
        {
            if(right_pressed())
            {
                incr_step(adc_reset, ADC_RESET_MIN, ADC_RESET_INF, ADC_RESET_STEP);
                redraw_menu = 1;
            }
            else if(left_pressed())
            {
                decr_step(adc_reset, ADC_RESET_MIN, ADC_RESET_INF, ADC_RESET_STEP);
                redraw_menu = 1;
            }
        }
        else if(menu_state == MENU_TRIGGER_VALUES)
        {
            if(right_pressed())
            {
                menu_state = MENU_TRIGGER_VALUES_EDIT;
                adc_reset = ADC_RESET_INF;
                adc_timer_pause();
                current = 0;
                redraw_menu = 1;
            }
        }
        else if(menu_state == MENU_TRIGGER_VALUES_EDIT)
        {
            if(right_pressed())
            {
                increment(current, 0, ALL_N - 1);
                redraw_menu = 1;
            }
            if(left_pressed())
            {
                decrement(current, 0, ALL_N - 1);
                redraw_menu = 1;
            }
        }

        else if(menu_state == MENU_VIEW_LCDSKIP)
        {
            if(right_pressed())
            {
                incr_step(lcd_skip, LCD_SKIP_MIN, LCD_SKIP_MAX, LCD_SKIP_STEP);
                redraw_menu = 1;
            }
            else if(left_pressed())
            {
                decr_step(lcd_skip, LCD_SKIP_MIN, LCD_SKIP_MAX, LCD_SKIP_STEP);
                redraw_menu = 1;
            }
        }
        else if(menu_state == MENU_VIEW_SPECTRUMYZOOM)
        {
            if(right_pressed())
            {
                increment(spectrum_y_zoom, SPECTRUM_ZOOM_MIN,
                          SPECTRUM_ZOOM_MAX_Y);
                redraw_menu = 1;
            }
            if(left_pressed())
            {
                decrement(spectrum_y_zoom, SPECTRUM_ZOOM_MIN,
                          SPECTRUM_ZOOM_MAX_Y);
                redraw_menu = 1;
            }
        }
        else if(menu_state == MENU_VIEW_SIGNALTYPE)
        {
            if(right_pressed())
            {
                increment(signal_type, DRAW_MIN, DRAW_MAX);
                redraw_menu = 1;
            }
            if(left_pressed())
            {
                decrement(signal_type, DRAW_MIN, DRAW_MAX);
                redraw_menu = 1;
            }
        }
        else if(menu_state == MENU_VIEW_TZOOM)
        {
            if(right_pressed())
            {
                increment(tzoom, TZOOM_MIN, TZOOM_MAX);
                redraw_menu = 1;
            }
            if(left_pressed())
            {
                decrement(tzoom, TZOOM_MIN, TZOOM_MAX);
                redraw_menu = 1;
            }
        }
        else if(menu_state == MENU_VIEW_VZOOM)
        {
            if(right_pressed())
            {
                increment(vzoom, VZOOM_MIN, VZOOM_MAX);
                redraw_menu = 1;
            }
            if(left_pressed())
            {
                decrement(vzoom, VZOOM_MIN, VZOOM_MAX);
                redraw_menu = 1;
            }
        }
        else if(menu_state == MENU_VIEW_VPOS)
        {
            if(right_pressed())
            {
                increment(vpos, VPOS_MIN, VPOS_MAX);
                redraw_menu = 1;
            }
            if(left_pressed())
            {
                decrement(vpos, VPOS_MIN, VPOS_MAX);
                redraw_menu = 1;
            }
        }
    }
    buttons_update();
}

inline uint8_t isrunning()
{
    return((pause_state()) && (menu_state != MENU_TRIGGER_VALUES_EDIT));
}

inline void buttons_update()
{
    right_state1 = right_state();
    left_state1 = left_state();
    top_state1 = top_state();
    up_state1 = up_state();
    down_state1 = down_state();
    running1 = isrunning();
}
