
#include "digital_io.h"

extern const int SEVEN_SEGMENT_DIGITS[];

void incrementDigits (int direction, int index, char digits[]);

void displayDigits(char digits[], char direction);

void init_shift_reg (char clock_pin, char shift_data_pin, char latch_pin);