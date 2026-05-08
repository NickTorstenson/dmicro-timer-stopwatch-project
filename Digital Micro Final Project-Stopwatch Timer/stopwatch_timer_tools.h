
#ifndef STOPWATCH_TIMER_TOOLS_H
#define STOPWATCH_TIMER_TOOLS_H

#include <stdint.h>
#include <stdbool.h>
#include "digital_io.h"
#include <util/delay.h>

#define F_CPU 16000000

// Shift Register Functions

extern const int SEVEN_SEGMENT_DIGITS[];

void incrementDigits (int direction, int index, char digits[]);

void displayDigits(char digits[], char direction);

void init_shift_reg (char clock_pin, char shift_data_pin, char latch_pin);


// LCD Configuration

#define LCD_MAX_COLS 16
#define LCD_MAX_ROWS 2

// LCD Commands

#define LCD_CLEAR_DISPLAY 0x01
#define LCD_RETURN_HOME 0x02
#define LCD_ENTRY_MODE_SET 0x04
#define LCD_DISPLAY_CONTROL 0x08
#define LCD_CURSOR_SHIFT 0x10
#define LCD_FUNCTION_SET 0x20
#define LCD_SET_CGRAM_ADDR 0x40
#define LCD_SET_DDRAM_ADDR 0x80

// Cursor Entry Modes

#define LCD_ENTRY_RIGHT 0x00
#define LCD_ENTRY_LEFT 0x02
#define LCD_ENTRY_SHIFT_INCREMENT 0x01
#define LCD_ENTRY_SHIFT_DECREMENT 0x00

// Display Control

#define LCD_DISPLAY_ON 0x04
#define LCD_DISPLAY_OFF 0x00
#define LCD_CURSOR_ON 0x02
#define LCD_CURSOR_OFF 0x00
#define LCD_BLINK_ON 0x01
#define LCD_BLINK_OFF 0x00

// Function Set
#define LCD_8BIT_MODE 0x10
#define LCD_4BIT_MODE 0x00
#define LCD_2_LINE 0x08
#define LCD_1_LINE 0x00
#define LCD_5x10_DOTS 0x04
#define LCD_5x8_DOTS 0x00

// LCD Pin Structure
typedef struct {
	uint8_t rs_pin; // Register Select pin
	uint8_t en_pin; // Enable pin
	uint8_t d4_pin; // Data pin 4
	uint8_t d5_pin; // Data pin 5
	uint8_t d6_pin; // Data pin 6
	uint8_t d7_pin; // Data pin 7
} LCD_Pins;

// Main LCD Structure
typedef struct {

	LCD_Pins pins; // Pin configuration
	uint8_t cols; // Number of columns 16
	uint8_t rows; // Number of rows 2
	uint8_t display_function; // Function set configuration
	uint8_t display_control; // Display control configuration
	uint8_t display_mode; // Entry mode configuration
	
} LCD;

bool lcd_init(LCD *lcd,
	uint8_t rs_pin,
	uint8_t en_pin,
	uint8_t d4_pin,
	uint8_t d5_pin,
	uint8_t d6_pin,
	uint8_t d7_pin,
	uint8_t cols,
	uint8_t rows);

void lcd_command(LCD *lcd, uint8_t value);

void lcd_send(LCD *lcd, const char *text);

void lcd_send_at(LCD *lcd, uint8_t col, uint8_t row, const char *text);

void lcd_clear(LCD *lcd);

void lcd_home(LCD *lcd);

void lcd_set_cursor(LCD *lcd, uint8_t col, uint8_t row);

void lcd_display(LCD *lcd, bool on);

void lcd_cursor(LCD *lcd, bool on);

void lcd_blink(LCD *lcd, bool on);

void lcd_create_char(LCD *lcd, uint8_t location, uint8_t charmap[]);

#endif