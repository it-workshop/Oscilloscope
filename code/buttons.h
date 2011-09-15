#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sfr_defs.h>
#include <avr/sleep.h>
#include <util/delay.h>

void buttons_process();
inline void buttons_update();

#endif
