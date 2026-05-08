
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
	0b00000000,  // 10 Blank
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

static void lcd_pulse_enable(LCD *lcd) {

	digitalWrite(lcd->pins.en_pin, 0);
	_delay_us(1);
	digitalWrite(lcd->pins.en_pin, 1);
	_delay_us(1);
	digitalWrite(lcd->pins.en_pin, 0);
	_delay_us(100);
}

static void lcd_write_4bits(LCD *lcd, uint8_t value) {

	digitalWrite(lcd->pins.d4_pin, (value >> 0) & 0x01);
	digitalWrite(lcd->pins.d5_pin, (value >> 1) & 0x01);
	digitalWrite(lcd->pins.d6_pin, (value >> 2) & 0x01);
	digitalWrite(lcd->pins.d7_pin, (value >> 3) & 0x01);
	lcd_pulse_enable(lcd);
}

static void lcd_send_byte(LCD *lcd, uint8_t value, uint8_t mode) {

	digitalWrite(lcd->pins.rs_pin, mode);

	// Send high nibble
	lcd_write_4bits(lcd, (value >> 4) & 0x0F);

	// Send low nibble
	lcd_write_4bits(lcd, value & 0x0F);
}

void lcd_command(LCD *lcd, uint8_t value) {
	lcd_send_byte(lcd, value, 0);
}

static void lcd_write_char(LCD *lcd, uint8_t value) {
	lcd_send_byte(lcd, value, 1);
}

bool lcd_init(LCD *lcd, uint8_t rs_pin, uint8_t en_pin, uint8_t d4_pin, uint8_t d5_pin, uint8_t d6_pin, uint8_t d7_pin, uint8_t cols, uint8_t rows) {

	if (!lcd) {
		return false;
	}

	// Initialize structure

	lcd->pins.rs_pin = rs_pin;
	lcd->pins.en_pin = en_pin;
	lcd->pins.d4_pin = d4_pin;
	lcd->pins.d5_pin = d5_pin;
	lcd->pins.d6_pin = d6_pin;
	lcd->pins.d7_pin = d7_pin;
	lcd->cols = cols;
	lcd->rows = rows;

	// Configure pins as outputs
	pinMode(lcd->pins.rs_pin, OUTPUT); // OUTPUT
	pinMode(lcd->pins.en_pin, OUTPUT);
	pinMode(lcd->pins.d4_pin, OUTPUT);
	pinMode(lcd->pins.d5_pin, OUTPUT);
	pinMode(lcd->pins.d6_pin, OUTPUT);
	pinMode(lcd->pins.d7_pin, OUTPUT);

	// Wait for LCD to power up
	_delay_ms(50);

	// Put LCD into 4-bit mode (initialization sequence)

	digitalWrite(lcd->pins.rs_pin, 0);
	digitalWrite(lcd->pins.en_pin, 0);

	// Initialization for 4-bit mode

	lcd_write_4bits(lcd, 0x03);
	_delay_ms(5);
	lcd_write_4bits(lcd, 0x03);
	_delay_ms(5);
	lcd_write_4bits(lcd, 0x03);
	_delay_us(150);
	lcd_write_4bits(lcd, 0x02); // Switch to 4-bit mode

	// 4-bit mode, 2 lines, 5x8 dots

	lcd->display_function = LCD_4BIT_MODE | LCD_2_LINE | LCD_5x8_DOTS;

	if (rows == 1) {
		lcd->display_function = LCD_4BIT_MODE | LCD_1_LINE | LCD_5x8_DOTS;
	}

	lcd_command(lcd, LCD_FUNCTION_SET | lcd->display_function);

	// display on, cursor off, blink off
	lcd->display_control = LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF;
	
	lcd_display(lcd, true);

	// Clear display
	lcd_clear(lcd);

	// left to right, no shift
	lcd->display_mode = LCD_ENTRY_LEFT | LCD_ENTRY_SHIFT_DECREMENT;
	lcd_command(lcd, LCD_ENTRY_MODE_SET | lcd->display_mode);

	return true;
}

void lcd_send(LCD *lcd, const char *text) {

	if (!lcd || !text) {
		return;
	}

	while (*text) {
		lcd_write_char(lcd, *text);
		text++;
	}
}

void lcd_send_at(LCD *lcd, uint8_t col, uint8_t row, const char *text) {

	if (!lcd) {
		return;
	}

	lcd_set_cursor(lcd, col, row);

	lcd_send(lcd, text);
}

void lcd_clear(LCD *lcd) {
	
	if (!lcd) {
		return;
	}

	lcd_command(lcd, LCD_CLEAR_DISPLAY);
}

void lcd_home(LCD *lcd) {

	if (!lcd) {
		return;
	}
	lcd_command(lcd, LCD_RETURN_HOME);

	_delay_ms(2);
}

void lcd_set_cursor(LCD *lcd, uint8_t col, uint8_t row) {

	uint8_t address;
	
	if (row == 0) {
		address = 0x00 + col;
	} else{
		address = 0x40 + col;
	}

	lcd_command(lcd, LCD_SET_DDRAM_ADDR | address);
	
	_delay_us(50);
}

void lcd_display(LCD *lcd, bool on) {

	if (!lcd) {
		return;
	}

	if (on) {
		lcd->display_control |= LCD_DISPLAY_ON;
		} else {
		lcd->display_control &= ~LCD_DISPLAY_ON;
	}

	lcd_command(lcd, LCD_DISPLAY_CONTROL | lcd->display_control);
}

void lcd_cursor(LCD *lcd, bool on) {

	if (!lcd) {
		return;
	}

	if (on) {
		lcd->display_control |= LCD_CURSOR_ON;
		} else {
		lcd->display_control &= ~LCD_CURSOR_ON;
	}

	lcd_command(lcd, LCD_DISPLAY_CONTROL | lcd->display_control);
}

void lcd_blink(LCD *lcd, bool on) {

	if (!lcd) {
		return;
	}

	if (on) {
		lcd->display_control |= LCD_BLINK_ON;
		} else {
		lcd->display_control &= ~LCD_BLINK_ON;
	}

	lcd_command(lcd, LCD_DISPLAY_CONTROL | lcd->display_control);
}

void lcd_create_char(LCD *lcd, uint8_t location, uint8_t charmap[]) {
	if (!lcd || location >= 8) {
		return;
	}
	
	location &= 0x7;
	lcd_command(lcd, LCD_SET_CGRAM_ADDR | (location << 3));
	for (int i = 0; i < 8; i++) {
		lcd_write_char(lcd, charmap[i]);
	}
}