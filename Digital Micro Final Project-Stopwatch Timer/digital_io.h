
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define LOW 0
#define HIGH 1
#define LSBFIRST 0
#define MSBFIRST 1

#include <stdio.h>

void pinMode(unsigned char pin, unsigned char mode);
void digitalWrite(unsigned char pin, unsigned char value);
unsigned char digitalRead(unsigned char pin);

void shiftOut(unsigned char dataPin,
	unsigned char clockPin,
	unsigned char bitOrder,
	unsigned char value
	);
	
void togglePin(unsigned char pin);

void init_joystick(char joystick_vrx, char joystick_vry, char joystick_sw);

void joystickInput(uint16_t *output);

void uart_init(unsigned long FCPU /* = 16000000UL */, unsigned long baud_rate /* = 9600 */);

void uart_tx(char c);

void uart_print(const char *s);

void adc_init(void);

unsigned int adc_read(uint8_t channel);