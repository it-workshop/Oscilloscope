#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sfr_defs.h>
#include <avr/sleep.h>
#include <util/delay.h>

#define top_state() (PINA & (1 << 2))
#define right_state() (PINA & (1 << 3))
#define left_state() (PINA & (1 << 4))
#define up_state() (PINA & (1 << 6))
#define down_state() (PINA & (1 << 7))
#define pause_state() (PINA & (1 << 5))

#define top_pressed() (!top_state() && top_state1)
#define right_pressed() (!right_state() && right_state1)
#define left_pressed() (!left_state() && left_state1)
#define up_pressed() (!up_state() && up_state1)
#define down_pressed() (!down_state() && down_state1)

inline uint8_t isrunning();
void buttons_process();
inline void buttons_update();

#endif
