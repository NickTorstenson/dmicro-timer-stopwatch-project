/*
 * Stopwatch_States.c
 *
 * Created: 4/21/2026 12:24:57 PM
 * Author : dahwilw
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000
#include <util/delay.h>
#include "stopwatch_timer_tools.h"

#define IDLE 0
#define COUNT_UP 1
#define SET_NUMBERS 2
#define COUNT_DOWN 3
#define RESET 4

#define Up 0
#define Down 1 

#define SHIFT_CLOCK_PIN 5
#define SHIFT_DATA_PIN 6
#define SHIFT_LATCH_PIN 4

#define LCD_RS 9
#define LCD_ENABLE 10
#define LCD_D4 11
#define LCD_D5 12
#define LCD_D6 13
#define LCD_D7 7

#define START_LAP_BUTTON 2
#define STOP_RESET_BUTTON 3
#define MODE_BUTTON 8
#define BuzzerPin 19



#define DELAY 10 //Number of ms between the smallest counts


void incrementDigits (int direction, int index, char digits[]);

void displayDigits(char digits[], char direction);


volatile int STATE = IDLE;
char Numbers[8] = {10,10,10,10, 10,10,10,10};
char ORDER = LSBFIRST;
volatile int Direction = 0; //1 counts up, 0 doesn't count, -1 counts down
volatile char Mode = Up; // Up or Down for counting up or down, defaults to up on startup
volatile char Count = 0;
volatile char PresentModeState = 0;
volatile char PastModeState = 0;
volatile char Timer_Count = 0;
char lap_records[50][19]; // 100 Lap strings, 16 bits long
uint8_t lap_index = 1;

LCD lcd;


int main(void)
{
	DDRB = 1<<5;
	EIMSK |= (1<<INT0); // Set PORTD2 interrupt (Start/Lap) (pin 2)
	pinMode(START_LAP_BUTTON, INPUT_PULLUP);
	EIMSK |= (1<<INT1); // Set PORTD3 interrupt (Stop/Reset) (pin 3)
	pinMode(STOP_RESET_BUTTON, INPUT_PULLUP);
	EICRA = 0b00001010; // Both are set to trigger on the falling edge
	
	PCICR |= (1<<PCIE0); //sets PC interrupt to look at PORTB
	PCMSK0 |= (1<<PCINT0); //sets PC interrupt to look at Pin0 (PORTB0) (Mode)
	pinMode(MODE_BUTTON, INPUT_PULLUP);
	
	TCNT1 = 0;
	TIMSK1 |= (1<<OCIE1A);
	TCCR1A = 0;
	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // CTC, pre-scaler 1024
	OCR1A = (15624); 
	
	init_shift_reg(SHIFT_CLOCK_PIN, SHIFT_DATA_PIN, SHIFT_LATCH_PIN);
	
	displayDigits(Numbers, ORDER); // call function to display numbers on 7 segs
	
	pinMode(BuzzerPin,OUTPUT);
	
	Joystick joystick;
	JoystickState joystick_state;
	
	adc_init();
	init_joystick(&joystick, 0, 1, 7);
	char timer_index = 7;
	char joystick_held = 0;
	//uart_init(F_CPU, 9600);
	
	lcd_init(&lcd, LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 16, 2);
	lcd_clear(&lcd);
	_delay_ms(10);
	
	//lcd_cursor(&lcd, 0);
	//lcd_blink(&lcd, 0);
	lcd_set_cursor(&lcd, 0, 0);
	lcd_send(&lcd, "STOPWATCH");
	
	
	sei();
	while(1){
		digitalWrite(BuzzerPin,LOW); //Buzzer off
		switch(STATE) {
			case IDLE:
				SLEEP_MODE_IDLE;
				continue;
				
			case COUNT_UP:
				displayDigits(Numbers, ORDER); // call function to display numbers on 7 segs
				incrementDigits(Direction, 7, Numbers);  //increments count
				_delay_ms(DELAY);
				TCNT1 = 0;
				Timer_Count = 0;
		
				continue;
				
			case SET_NUMBERS:
				//Set Numbers code
				
				displayDigits(Numbers, ORDER);
				
				joystick_state = read_joystick(&joystick);
				
				if ((joystick_state.x < 450) && (!joystick_held)){
					joystick_held = 1;
					if (timer_index == 0){
						timer_index = 7;
					} else{
						timer_index = (timer_index-1);
					}
				} else if ((joystick_state.x > 560) && (!joystick_held)) {
					joystick_held = 1;
					if (timer_index == 7){
						timer_index = 0;
						} else{
						timer_index = (timer_index+1);
					}
				} else if ((joystick_state.y < 450) && (!joystick_held)){
					joystick_held = 1;
					if (Numbers[timer_index] == 9){
						Numbers[timer_index] = 0;	
					}else{
						Numbers[timer_index] = (Numbers[timer_index]+1);
					}
				} else if ((joystick_state.y > 560) && (!joystick_held)){
					joystick_held = 1;
					if (Numbers[timer_index] == 0){
						Numbers[timer_index] = 9;
						}else{
						Numbers[timer_index] = (Numbers[timer_index]-1);
					}
				} else if(((joystick_state.x < 560)&&(joystick_state.x > 450)) && ((joystick_state.y < 560)&&(joystick_state.y > 450))){
					joystick_held = 0;
				}
				displayDigits(Numbers, ORDER);
				//_delay_ms(100);
				TCNT1 = 0;
				Timer_Count = 0;
				
				continue;
				
			case COUNT_DOWN:
				if ( ((Numbers[0] == 0) || (Numbers[0] == 10)) && ((Numbers[1] == 0) || (Numbers[1] == 10)) && ((Numbers[2] == 0) || (Numbers[2] == 10)) && ((Numbers[3] == 0) || (Numbers[3] == 10)) && ((Numbers[4] == 0) || (Numbers[4] == 10)) && ((Numbers[5] == 0) || (Numbers[5] == 10)) && ((Numbers[6] == 0) || (Numbers[6] == 10)) && ((Numbers[7] == 0) || (Numbers[7] == 10)) ) {
					Direction = 0;
					digitalWrite(BuzzerPin,HIGH); //Buzzer on
				}
				else{
					Direction = -1;
				}
			
				displayDigits(Numbers, ORDER); // call function to display numbers on 7 segs
				incrementDigits(Direction, 7, Numbers);  //increments count
				_delay_ms(DELAY);
				
				TCNT1 = 0;
				Timer_Count = 0;
			
				continue;
				
			case RESET:
				Numbers[0] = 10; //set entire array to blank
				Numbers[1] = 10;
				Numbers[2] = 10;
				Numbers[3] = 10;
				Numbers[4] = 10;
				Numbers[5] = 10;
				Numbers[6] = 10;
				Numbers[7] = 10;
				displayDigits(Numbers, ORDER);
				
				
				if (Mode == Down) {
					Numbers[0] = 0; //set entire array to zero
					Numbers[1] = 0;
					Numbers[2] = 0;
					Numbers[3] = 0;
					Numbers[4] = 0;
					Numbers[5] = 0;
					Numbers[6] = 0;
					Numbers[7] = 0;
					displayDigits(Numbers, ORDER);
					STATE = SET_NUMBERS;
				}
				continue;
				
			default:
				STATE = IDLE;
				continue;
		}
		
		
	
	}
}



ISR(INT0_vect){ // Start/Lap Button
	if (Direction == 0){ //Start
		if (Mode == Up){
			Direction = 1;
			STATE = COUNT_UP;
			
		}
		else {
			Direction = -1;
			STATE = COUNT_DOWN;
		}
	}
	else{ // Lap
		// Lap Code Here
		// Take in Numbers array, split indexes up and append to Lap (array) needs to be in format "LAPX|HH:MM:ss:mm"
		snprintf(lap_records[lap_index], sizeof(lap_records[lap_index]), "%d| %d%d:%d%d:%d%d:%d%d", 
		lap_index++, 
		Numbers[0] == 10 ? 0: Numbers[0], Numbers[1] == 10 ? 0: Numbers[1], 
		Numbers[2] == 10 ? 0: Numbers[2], Numbers[3] == 10 ? 0: Numbers[3], 
		Numbers[4] == 10 ? 0: Numbers[4], Numbers[5] == 10 ? 0: Numbers[5], 
		Numbers[6] == 10 ? 0: Numbers[6], Numbers[7] == 10 ? 0: Numbers[7]); // I tried being clever, if not working update lap index on its own after
		
		// TODO: Update the LCD with lap string
		lcd_clear(&lcd);
		
		_delay_ms(5);
		
		lcd_set_cursor(&lcd, 0, 0);
		lcd_send(&lcd, lap_records[lap_index]);
		//uart_print(lap_records[lap_index]);
	}
}

ISR(INT1_vect){ // Stop/Reset Button
	if (Direction != 0){//Stop
		Direction = 0;
	}
	else{ // Reset
		lap_index = 1;
		
		//lcd_clear(&lcd);
		//_delay_ms(5);
		//lcd_set_cursor(&lcd, 0, 0);
		STATE = RESET;
	}
}

ISR(PCINT0_vect){ // Mode (Could switch to a timer set to an "external timer" triggered by the button")
	
	if (Count==1){
		if (Mode == Up){
			Mode = Down;
		}
		else{
			Mode = Up;
		}
		Count = 0;
		
	}
	
	else {
		Count++;
	}
	//lcd_clear(&lcd);
	//_delay_ms(5);
}

ISR(TIMER1_COMPA_vect){ // Idle Timer
	if (Timer_Count == 10){
		Timer_Count = 0;
		STATE = IDLE;
	}
	else {
		Timer_Count++;
	}
}