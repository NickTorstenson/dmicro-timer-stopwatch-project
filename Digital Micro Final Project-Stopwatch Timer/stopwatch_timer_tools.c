
#include "stopwatch_timer_tools.h"

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

