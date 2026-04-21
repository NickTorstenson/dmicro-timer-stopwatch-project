
#include "stopwatch_timer_tools.h"
#include "digital_io.h"

const int SEVEN_SEGMENT_DIGITS[] = {
	0b11111100,  // 0
	0b01100000,  // 1
	0b11011010,  // 2
	0b11110010,  // 3
	0b01100110,  // 4
	0b10110110,  // 5
	0b10111110,  // 6
	0b11100000,  // 7
	0b11111110,  // 8
	0b11110110,  // 9
	0b00000000,  // Blank
};

char SHIFT_CLOCK_PIN = 0;
char SHIFT_DATA_PIN = 0;
char SHIFT_LATCH_PIN = 0;

void init_shift_reg (char clock_pin, char shift_data_pin, char latch_pin) {
	SHIFT_CLOCK_PIN = clock_pin;
	SHIFT_DATA_PIN = shift_data_pin;
	SHIFT_LATCH_PIN = latch_pin;
	
	pinMode(SHIFT_CLOCK_PIN, OUTPUT);
	pinMode(SHIFT_DATA_PIN, OUTPUT);
	pinMode(SHIFT_LATCH_PIN, OUTPUT);
	digitalWrite(SHIFT_LATCH_PIN, LOW);
}

void displayDigits(char digits[], char direction) {
	// Direction: MSBFIRST or LSBFIRST
	digitalWrite(SHIFT_LATCH_PIN, LOW);
	for (int i = 0; i < 8; i++){
		shiftOut(SHIFT_DATA_PIN, SHIFT_CLOCK_PIN, direction, SEVEN_SEGMENT_DIGITS[i]);
	}
	// display all at once
	digitalWrite(SHIFT_LATCH_PIN, HIGH);
	digitalWrite(SHIFT_LATCH_PIN, LOW);
}

void incrementDigits (int direction, int index, char digits[]) {
	/*
	direction:
		1: stopwatch mode (count up)
		0: paused (will increment by 0)
		-1: timer mode (count down)
	digits:
		indexes 0, 1, 3, 5, 6, 7 abide by 0-9
			indexes 2, 4 are 0-5
		array of digits to be shifted and displayed
		length = 8
		MSB -> index 0
		LSB -> index 7
	*/
	
	if (direction == 0 || index < 0) // Don't compute
	{
		return;
	}
	
	if (index == 3 || index == 5 || index == 6 || index == 7) {
		if (direction > 0 && digits[index] == 9) {
			digits[index] = 0;
			incrementDigits(direction, index-1, digits);
		} else if (direction < 0 && digits[index] == 0) {
			digits[index] = 9;
			incrementDigits(direction, index-1, digits);
		} else {
			digits[index] += direction;
		}
	} else {
		if (direction > 0 && digits[index] == 5) {
			digits[index] = 0;
			incrementDigits(direction, index-1, digits);
		} else if (direction < 0 && digits[index] == 0) {
			digits[index] = 5;
			incrementDigits(direction, index-1, digits);
		} else {
			digits[index] += direction;
		}
	}
}

