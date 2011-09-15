#ifndef DRAW_H
#define DRAW_H

void draw_signal();

inline void fft_maxfreq();
inline void dfreq();
void osd();
void welcome();

inline uint8_t todisplay(uint16_t a);
inline void dfreq_only(uint8_t x,uint8_t y);

#define osd_delay() _delay_ms(200)

#endif
