#include <avr/eeprom.h>
#include <util/delay.h>
#include "lcd.h"

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

void lcd_databits(uint8_t cmd,uint8_t src) {
  DATAPORT=src;
  if(cmd==SEND_DATA) CMDPORT|=DI;
  else CMDPORT&=~DI;
  CMDPORT|=EN;
  _delay_us(8);
  CMDPORT&=~EN;
  CMDPORT&=~DI;
  DATAPORT=0;
}

void lcd_goto_xblock(uint8_t s) { //s={0,1...127}
  if(s>=64) {
    CMDPORT|=CSEL2;
    CMDPORT&=~CSEL1;
    lcd_databits(SEND_CMD,(1<<6)|(s-64));
  }
  else {
    CMDPORT|=CSEL1;
    CMDPORT&=~CSEL2;
    lcd_databits(SEND_CMD,(1<<6)|s);
  }
}

void lcd_goto_yblock(uint8_t s) { //s={0,1...7}
  lcd_databits(SEND_CMD,0xb8|s);
}
void lcd_goto(uint8_t x,uint8_t y) {
	lcd_goto_xblock(x);
	lcd_goto_yblock(y);
}
void lcd_block(uint8_t x,uint8_t y,uint8_t block) {
  lcd_goto_xblock(x);
  lcd_goto_yblock(y);
  lcd_databits(SEND_DATA,block);
}

void lcd_all(uint8_t w) {
  unsigned static int i,si;
  CMDPORT|=CSEL1;
  CMDPORT|=CSEL2;
  for(i=0;i<8;i++) {
   lcd_goto_yblock(i);
   for(si=0;si<64;si++)
     lcd_databits(SEND_DATA,w);
  }
}

void lcd_init() {
  CMDDDR=CSEL1|CSEL2|RW|RS|EN|DI;
  CMDPORT=0x00;
  DATADDR=0xff;
  DATAPORT=0x00;
  CMDPORT=RS|CSEL1|CSEL2;
  _delay_ms(100);
  lcd_databits(SEND_CMD,0x3f);
  lcd_databits(SEND_CMD,0xc0);
  lcd_databits(SEND_CMD,0x40);
  lcd_databits(SEND_CMD,0xb8);
  lcd_all(0);
}
void lcd_sym(uint8_t sym,uint8_t x,uint8_t y) {
	static uint8_t t;
	for(t=0;t<5;t++) {
		lcd_goto(x,y);
		lcd_databits(SEND_DATA,eeprom_read_byte((uint8_t*) (5*sym+t)));
		x++;
	}
	lcd_goto(x,y);
	lcd_databits(SEND_DATA,0);
}


void lcd_str(char* s,uint8_t x,uint8_t y) {
	while(*s) {
		if(x>=122||(*s=='\n')) {
			x=0;
			y++;
			if(*s=='\n') s++;
		}
		lcd_sym(*s++,x,y);
		if(x<122) x+=6;
	}
}

uint8_t lcd_read(uint8_t x,uint8_t y) {
	lcd_goto(x,y);
	CMDPORT|=DI|RW;
	_delay_us(6);
	CMDPORT&=~(DI|RW);
	return(DATAPORT);
	
}

inline void lcd_pixel(uint8_t x,uint8_t y) {
    lcd_block(x,(y-y%8)/8,1<<(y%8));
}

void lcd_pixel_share(uint8_t x,uint8_t y) {
	lcd_block(x,(y-y%8)/8,(1<<(y%8))|lcd_read(x,(y-y%8)/8));
}

void lcd_line_from_bottom(uint8_t x1,uint8_t y1) {
	int y=0;
	uint8_t res=0;
	for(y=7;y>(7-((y1-y1%8)/8));y--) {
		lcd_goto_xblock(x1);
		lcd_goto_yblock(y);
		lcd_databits(SEND_DATA,0xff);
	}
	lcd_goto_xblock(x1);
	lcd_goto_yblock(y);
	for(y=7;y>=(7-y1%8);y--) {
		res|=1<<y;
	}
	if(res) lcd_databits(SEND_DATA,res);
}

void lcd_pixel_line_from_left(uint8_t y, uint16_t v) {
	uint16_t s=(v-v%8)/8,x;
	uint8_t r;
	for(x=0;x<s;x++) {
		lcd_goto(x,y);
		lcd_databits(SEND_DATA,0xff);
	}
	lcd_goto(x,y);
	s=v%8;
	r=0;
	for(x=0;x<=s;x++) {
		r|=1<<(7-x);
	}
	lcd_databits(SEND_DATA,r);
}
void lcd_num_from_right(unsigned int maxx,unsigned int y,uint32_t n) {
	short unsigned int m,x=maxx-FONT_SIZE-1;
	if(!n) {
		lcd_sym('0',x,y);
	}
	while(n) {
		m=n%10;
		lcd_sym('0'+m,x,y);
		x-=FONT_SIZE+1;
		n-=m;
		n/=10;
	}
}
inline uint8_t lcd_yblockof(uint8_t y) {
	return((y-y%8)/8);
}
void lcd_constx_line(uint8_t x,uint8_t ymin,uint8_t ymax) {
	static uint8_t buf;
	buf=0;
	while(ymin<=ymax) {
		buf|=1<<(ymin%8);
		if(((ymin%8)==7)||(ymin==ymax)) {
			lcd_goto_xblock(x);
			lcd_goto_yblock(lcd_yblockof(ymin));
			lcd_databits(SEND_DATA,buf);
			buf=0;
		}
		ymin++;
	}
}
