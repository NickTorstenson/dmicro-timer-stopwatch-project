
#ifndef DIGITAL_IO_H
#define DIGITAL_IO_H

#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define LOW 0
#define HIGH 1
#define LSBFIRST 0
#define MSBFIRST 1

#include <avr/io.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
	uint16_t x; // will be the position value of the x axis
	uint16_t y; // will be the position value of the y axis
	bool pressed; // will be the pressed state of the joystick button
} JoystickState;

typedef struct {
	uint8_t x_ch;   // ADC Channel for X (0 for ADC0)
	uint8_t y_ch;   // ADC Channel for Y (1 for ADC1)
	uint8_t sw_pin; // GPIO Pin for the switch (Standard pin #)
} Joystick;

void pinMode(unsigned char pin, unsigned char mode);

void digitalWrite(unsigned char pin, unsigned char value);

unsigned char digitalRead(unsigned char pin);

void shiftOut(unsigned char dataPin, unsigned char clockPin, unsigned char bitOrder, unsigned char value);
	
void togglePin(unsigned char pin);

void init_joystick(Joystick *joystick, uint8_t x_channel, uint8_t y_channel, uint8_t sw_pin);

JoystickState read_joystick(const Joystick *joystick);

void uart_init(unsigned long FCPU /* = 16000000UL */, unsigned long baud_rate /* = 9600 */);

void uart_tx(char c);

void uart_print(const char *s);

void adc_init(void);

unsigned int adc_read(uint8_t channel);

#endif