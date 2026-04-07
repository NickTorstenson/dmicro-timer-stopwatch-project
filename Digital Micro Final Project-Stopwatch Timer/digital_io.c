
#include <avr/io.h>
#include "digital_io.h"

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