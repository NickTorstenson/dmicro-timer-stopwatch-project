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

void displayDigits(char d1, char d2);

const char CLOCK = 2;
const char SHIFT_DATA = 3;
const char LATCH_CLOCK = 4;


char value = 8;


const int DIGITS[] = {
	0b11111100,  // 0
	0b01100000,  // 1
	0b11011010,  // 2
	0b11110010,  // 3
	0b01100110,  // 4
	0b10110110,  // 5
	0b10111110,  // 6
	0b11100000,  // 7
	0b11111110,  // 8
	0b11110110   // 9
};

int main(void)
{
    pinMode(CLOCK, OUTPUT);
    pinMode(SHIFT_DATA, OUTPUT);
    pinMode(LATCH_CLOCK, OUTPUT);
    digitalWrite(LATCH_CLOCK, LOW);
	char i = 0;
    while (1) 
    {
		displayDigits(i++, 0);
		_delay_ms(200);
		if (i > 9) 
			i = 0;
    }
}


int decimalToSevenSegment(char decimal_num) {
	return DIGITS[decimal_num];
}

void displayDigits(char d1) {
	digitalWrite(LATCH_CLOCK, LOW);
	
	// shift out rightmost digit first
	//shiftOut(SHIFT_DATA, CLOCK, MSBFIRST, DIGITS[d3]);
	//shiftOut(SHIFT_DATA, CLOCK, LSBFIRST, DIGITS[d2]);
	shiftOut(SHIFT_DATA, CLOCK, LSBFIRST, DIGITS[d1]);
	
	// latch all at once
	digitalWrite(LATCH_CLOCK, HIGH);
	digitalWrite(LATCH_CLOCK, LOW);
}





