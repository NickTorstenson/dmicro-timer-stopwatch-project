
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

void init_joystick(Joystick *joystick, uint8_t x_channel, uint8_t y_channel, uint8_t sw_pin) {
	/* 
	x_channel/y_channel: analog channel (A0 = 0, A1 = 1, etc)
	sw_pin: digital input pin for the switch
	*/
	
	joystick->x_ch = x_channel;
	joystick->y_ch = y_channel;
	joystick->sw_pin = sw_pin;
	
	pinMode(sw_pin, INPUT_PULLUP);
}

JoystickState read_joystick(const Joystick *joystick) { // outputs a struct with the raw joystick values
	/*
	USAGE:
		Joystick joystick; // create a joystick "object"
		JoystickState joystick_state; // create a joystick state with x, y, and pressed values
		
		adc_init();
		init_joystick(&joystick, 0, 1, 2);
		joystick_state = read_joystick(&joystick);
		
	OUTPUT:
		JoystickState {
			x: x position of the joystick 0-1023, center 516
			y: y position of the joystick 0-1023, center 510
			pressed: 0 not pressed, 1 pressed
		}
	
	DEBUG USAGE:
		Joystick joystick;
		JoystickState joystick_state;
		
		adc_init();
		init_joystick(&joystick, 0, 1, 2);
		uart_init(F_CPU, 9600);
		
		while(1) {
			joystick_state = read_joystick(&joystick);
			
			char buffer[80];
			sprintf(buffer, "\nX:%u Y:%u SW:%u CHX:%u CHY:%u\n",
			joystick_state.x,
			joystick_state.y,
			joystick_state.pressed,
			joystick.x_ch,
			joystick.y_ch);

			uart_print(buffer);
		}
	*/
	
	JoystickState state;
	
	state.x = adc_read(joystick->x_ch);
	state.y = adc_read(joystick->y_ch);
	state.pressed = (digitalRead(joystick->sw_pin) == LOW);
	
	return state;
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