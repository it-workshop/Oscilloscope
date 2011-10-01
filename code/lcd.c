#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"

/* configuration */
//lcd's setup pins PORT
#define CMDPORT PORTD
#define CMDDDR DDRD

#define DATAPORT PORTC
#define DATADDR DDRC
#define DATAPIN PINC

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
	CMDDDR|=CSEL1|CSEL2|RW|RS|EN|DI;
	DATADDR=0xff;
	DATAPORT=0x00;
	CMDPORT|=CSEL1|CSEL2|RS;
	CMDPORT&=~(RW|EN|DI);
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
		if(x<122) {
			if(*(s-1)==' ') x+=5;
			else x+=6;
		}
	}
}

uint8_t lcd_read() {
	uint8_t res;
	cli();
	CMDPORT|=DI|RW|RS;
	DATAPORT=0;
	DATADDR=0x00;
	
	CMDPORT|=EN;
	_delay_us(6);
	res=DATAPIN;
	
	CMDPORT&=~EN;
	
	CMDPORT&=~(DI|RW);
	DATADDR=0xff;
	
	sei();
	return(res);
}

inline void lcd_pixel(uint8_t x,uint8_t y) {
	lcd_block(x,(y-y%8)/8,1<<(y%8));
}

void lcd_pixel_share(uint8_t x,uint8_t y) {
	lcd_goto(x,y/8);
	lcd_read();
	lcd_block(x,y/8,lcd_read()|(1<<(y%8)));
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

void lcd_float_from_right(unsigned int maxx,unsigned int y,uint32_t n,uint8_t z) {
	short unsigned int m,x=maxx-FONT_SIZE-1,t=0;
	if(!n) {
		lcd_sym('0',x,y);
	}
	while(n) {
		if(x==(maxx-z*((FONT_SIZE+1)))) {
			lcd_sym('.',x,y);
			x-=FONT_SIZE+1;
			t=1;
			continue;
		}
		m=n%10;
		lcd_sym('0'+m,x,y);
		x-=FONT_SIZE+1;
		n-=m;
		n/=10;
	}
	if(!t&&n) {
		lcd_str("0.",x-(FONT_SIZE+1),y);
	}
}

inline uint8_t lcd_yblockof(uint8_t y) {
	return((y-y%8)/8);
}

void lcd_constx_line(uint8_t x,uint8_t ymin,uint8_t ymax) {
	static uint8_t buf;
	if(ymin>ymax) {
		buf=ymax;
		ymax=ymin;
		ymin=buf;
	}
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
