#ifndef DRAW_H
#define DRAW_H

#define DRAW_SIGNAL_DUAL 1
#define DRAW_SIGNAL 2

#define DRAW_LINES 1
#define DRAW_DOTS 2
#define DRAW_MIN DRAW_LINES
#define DRAW_MAX DRAW_DOTS


void draw_signal(uint8_t draw_what,uint8_t draw_type);

inline void fft_maxfreq();
inline void dfreq();
void osd();
void welcome();

uint8_t todisplay(uint8_t a);
inline void dfreq_only(uint8_t x,uint8_t y);

#define osd_delay() _delay_ms(200)

#endif
