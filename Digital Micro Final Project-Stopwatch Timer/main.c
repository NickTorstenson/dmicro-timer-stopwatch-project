/*
 * Digital Micro Final Project-Stopwatch Timer.c
 *
 * Created: 4/7/2026 11:14:16 AM
 * Author : tornich and dahwill
 */ 

#include <avr/io.h>
#define F_CPU 16000000
#include <avr/io.h>
#include "digital_io.h"
#include <util/delay.h>
#include "stopwatch_timer_tools.h"


const char CLOCK = 2;
const char SHIFT_DATA = 3;
const char LATCH_CLOCK = 4;



void usart_init (void) {
	UCSR0B = (1<<TXEN0); // transmit enable
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00); // 8bit data length
	UBRR0L = 103; // 9600 baud
}

void usart_send (unsigned char ch) {
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = ch;
}

int main(void)
{
	char test_digits[] = {0,0,5,9,5,9,9,5};
	
	usart_init();
	init_shift_reg(CLOCK, SHIFT_DATA, LATCH_CLOCK);
	
    while (1) 
    {
		
		incrementDigits(-1, 7, test_digits);
		for (int i = 0; i <= 7; i++) {
			if (i == 2 | i == 4 | i == 6) usart_send(':');
			usart_send(test_digits[i] + '0'); // convert to ASCII
		}
		usart_send('\r');
		usart_send('\n');
		_delay_ms(1000);
    }
	return 0;
}








