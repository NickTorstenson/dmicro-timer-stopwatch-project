
#include <avr/io.h>
#include "digital_io.h"
#include <stdio.h>

static char JOYSTICK_VRX;
static char JOYSTICK_VRY;
static char JOYSTICK_SW;

void togglePin(unsigned char pin) {
	digitalWrite(pin, !digitalRead(pin));
}

void shiftOut(unsigned char dataPin,
				unsigned char clockPin,
				unsigned char bitOrder,
				unsigned char value)
{
	unsigned char i;
	unsigned char bit;
	digitalWrite(clockPin, LOW);
	for (i = 0; i < 8; i++)
	{
		if (bitOrder == LSBFIRST) {
			bit = (value >> i) & 0x01;
		}
		else {
			bit = (value >> (7 - i)) & 0x01;
		}
		if (bit == 1){
			digitalWrite(dataPin, HIGH);
		}
		else {
			digitalWrite(dataPin, LOW);
		}
		digitalWrite(clockPin, HIGH);
		digitalWrite(clockPin, LOW);
	}
}


void pinMode(unsigned char pin, unsigned char mode)
{
	if (pin < 8) {
		if (mode == OUTPUT)
		DDRD |= (1 << pin);
		else {
			DDRD &= ~(1 << pin);
			if (mode == INPUT_PULLUP)
			PORTD |= (1 << pin);
		}
	}
	else if (pin < 14) {
		unsigned char bit = pin - 8;

		if (mode == OUTPUT)
		DDRB |= (1 << bit);
		else {
			DDRB &= ~(1 << bit);
			if (mode == INPUT_PULLUP)
			PORTB |= (1 << bit);
		}
	}
}

void digitalWrite(unsigned char pin, unsigned char value)
{
	if (pin < 8) {
		if (value == HIGH)
		PORTD |= (1 << pin);
		else
		PORTD &= ~(1 << pin);
	}
	else if (pin < 14) {
		unsigned char bit = pin - 8;

		if (value == HIGH)
		PORTB |= (1 << bit);
		else
		PORTB &= ~(1 << bit);
	}
}

unsigned char digitalRead(unsigned char pin)
{
	if (pin < 8)
	return (PIND & (1 << pin)) ? HIGH : LOW;

	else if (pin < 14) {
		unsigned char bit = pin - 8;
		return (PINB & (1 << bit)) ? HIGH : LOW;
	}

	return LOW;
}

void init_joystick(char joystick_vrx, char joystick_vry, char joystick_sw) {
	JOYSTICK_VRX = joystick_vrx;
	pinMode(joystick_vrx, INPUT);
	JOYSTICK_VRY = joystick_vry;
	pinMode(joystick_vry, INPUT);
	JOYSTICK_SW = joystick_sw;
	pinMode(joystick_sw, INPUT_PULLUP);
}

void joystickInput(uint16_t *output) { // changes the values in the given array to the inputs
	/*
		Output from 0-667, Center: A0 - 335/336, A1 - 331/332
		
	USAGE:
	
		init_joystick(23, 24, 4);
		
		uart_init(F_CPU, 9600);
		
		adc_init();
		
		while (1) {
			
			uint16_t joy[2];
			joystickInput(joy);

			char buffer[32];
			sprintf(buffer, "X:%u Y:%u\n", joy[0], joy[1]);
			uart_print(buffer);
		}
	*/
	output[0] = adc_read(0);
	output[1] = adc_read(1);
}


void uart_init(unsigned long FCPU, unsigned long baud_rate) {
	unsigned long ubrr = ((FCPU / 16 / baud_rate) - 1);
	UBRR0H = (char)(ubrr >> 8);
	UBRR0L = (char)ubrr;
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);           /* enable TX and RX        */
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);         /* 8-N-1 frame format      */
}

void uart_tx(char c) {
	while (!(UCSR0A & (1 << UDRE0)));               /* wait for empty buffer   */
	UDR0 = c;
}

void uart_print(const char *s) {
	while (*s) {
		if (*s == '\n') uart_tx('\r');              /* translate LF to CRLF    */
		uart_tx(*s++);
	}
}

void adc_init(void) {
	ADMUX = (1 << REFS0); // AVCC reference

	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // prescaler 128

	DIDR0 |= (1 << ADC0D) | (1 << ADC1D); // disable digital input on A0/A1
}

uint16_t adc_read(uint8_t channel) {

	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}