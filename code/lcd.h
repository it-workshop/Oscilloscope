#ifndef LCD_H
#define LCD_H
#include <stdio.h>
#define lcd_arrows(x,y) lcd_str("< >",x,y)
/* configuration */
//lcd's setup pins PORT
#define CMDPORT PORTD
#define CMDDDR DDRD
#define DATAPORT PORTC
#define DATADDR DDRC

#define CSEL1 (1<<2)
#define CSEL2 (1<<3)
#define RW (1<<4)
#define DI (1<<5)
#define EN (1<<6)
#define RS (1<<7)
/* end of configuration */

#define DISPLAY_X 128
#define DISPLAY_Y 64
#define FONT_SIZE 5

#define SEND_CMD 1
#define SEND_DATA 2

void lcd_databits(uint8_t cmd,uint8_t src);
void lcd_goto_xblock(uint8_t s);
void lcd_goto_yblock(uint8_t s);
void lcd_goto(uint8_t x,uint8_t y);
void lcd_block(uint8_t x,uint8_t y,uint8_t block);
void lcd_all(uint8_t w);
void lcd_init();
void lcd_sym(uint8_t sym,uint8_t x,uint8_t y);
void lcd_str(char* s,uint8_t x,uint8_t y);
uint8_t lcd_read(uint8_t x,uint8_t y);
inline void lcd_pixel(uint8_t x,uint8_t y);
void lcd_pixel_share(uint8_t x,uint8_t y);
void lcd_line_from_bottom(uint8_t x1,uint8_t y1);
void lcd_pixel_line_from_left(uint8_t y, uint16_t v);
void lcd_num_from_right(unsigned int maxx,unsigned int y,uint32_t n);
inline uint8_t lcd_yblockof(uint8_t y);
void lcd_constx_line(uint8_t x,uint8_t ymin,uint8_t ymax);
#endif
