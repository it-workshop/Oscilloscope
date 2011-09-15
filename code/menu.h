#ifndef MENU_H
#define MENU_H

#define MENU_SHIFT 4
#define MENU_NONE 0

//menu items
#define MENU_EXIT (1<<MENU_SHIFT)

#define MENU_MODE (2<<MENU_SHIFT) //MODES,UART
#define MENU_MODE_MODES (MENU_MODE|1)
#define MENU_MODE_UART (MENU_MODE|2)
#define MENU_MODE_MAX MENU_MODE_UART

#define MENU_ADC 3<<MENU_SHIFT //FREQ,INPUT
#define MENU_ADC_FREQ (MENU_ADC|1)
#define MENU_ADC_INPUT (MENU_ADC|2)
#define MENU_ADC_MAX MENU_ADC_INPUT

#define MENU_TRIGGER (4<<MENU_SHIFT) //CHECK,ERROR,RESET
#define MENU_TRIGGER_CHECK (MENU_TRIGGER|1)
#define MENU_TRIGGER_ERROR (MENU_TRIGGER|2)
#define MENU_TRIGGER_RESET (MENU_TRIGGER|3)
#define MENU_TRIGGER_MAX MENU_TRIGGER_RESET

#define MENU_VIEW (5<<MENU_SHIFT) //LCDSKIP,SPECTRUMYZOOM
#define MENU_VIEW_LCDSKIP (MENU_VIEW|1)
#define MENU_VIEW_SPECTRUMYZOOM (MENU_VIEW|2)
#define MENU_VIEW_MAX MENU_VIEW_SPECTRUMYZOOM

#define MENU_ABOUT (6<<MENU_SHIFT)

#define MENU_MAX (6<<MENU_SHIFT)

#define menu_attoplevel() !(menu_state&(0xff>>(8-MENU_SHIFT)))
#define menu_topstate() (menu_state&(0xff<<MENU_SHIFT))


//menu_state bytes:
//0..3 4..7
//top  second

void menu_top_change();
uint8_t menu_second_max();
void draw_menu();

#endif
