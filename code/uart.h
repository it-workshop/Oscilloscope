#ifndef UART_H
#define UART_H

#ifdef UDR0
	#define UBRRH UBRR0H
	#define UBRRL UBRR0L
	#define UDR UDR0

	#define UCSRA UCSR0A
	#define UDRE UDRE0
	#define RXC RXC0

	#define UCSRB UCSR0B
	#define RXEN RXEN0
	#define TXEN TXEN0
	#define RXCIE RXCIE0

	#define UCSRC UCSR0C
	#define URSEL 
	#define UCSZ0 UCSZ00
	#define UCSZ1 UCSZ01
	#define UCSRC_SELECT 0
#else
	#define UCSRC_SELECT (1 << URSEL)
#endif

#define BAUD 57600UL
#define UBRRVAL (F_CPU/(BAUD*16)-1)

#define uart_available() (UCSRA & (1 << RXC))
#define uart_available_tx() (UCSRA&(1<<UDRE))

#define UART_ACTION_B_RC0   1
#define UART_ACTION_B_RC1   2
#define UART_ACTION_DEFAULT 0

void uart_flush();
void uart_init();
void uart_putc(uint8_t c);
uint8_t uart_getc();
void uart_action(uint8_t c);
void uart_close();

#endif
