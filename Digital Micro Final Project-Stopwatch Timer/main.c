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
#define SHIFT_LATCH_PIN 7
#define BuzzerPin 11

#define DELAY 10 //Number of ms between the smallest counts


void incrementDigits (int direction, int index, char digits[]);

void displayDigits(char digits[], char direction);


volatile int STATE = IDLE;
char Numbers[8] = {0,0,0,0, 0,0,0,0};
char ORDER = LSBFIRST;
volatile int Direction = 0; //1 counts up, 0 doesn't count, -1 counts down
volatile char Mode = Up; // Up or Down for counting up or down, defaults to up on startup
volatile char Count = 0;
volatile char PresentModeState = 0;
volatile char PastModeState = 0;
volatile char Timer_Count = 0;
char lap_records[100][16]; // 100 Lap strings, 16 bits long
uint8_t lap_index = 0;




int main(void)
{
	DDRB = 1<<5;
	EIMSK |= (1<<INT0); // Set PORTD2 interrupt (Start/Lap) (pin 2)
	pinMode(2, INPUT_PULLUP);
	EIMSK |= (1<<INT1); // Set PORTD3 interrupt (Stop/Reset) (pin 3)
	pinMode(3, INPUT_PULLUP);
	EICRA = 0b00001010; // Both are set to trigger on the falling edge
	
	PCICR |= (1<<PCIE0); //sets PC interrupt to look at PORTB
	PCMSK0 |= (1<<PCINT0); //sets PC interrupt to look at Pin0 (PORTB0) (Mode)
	pinMode(8, INPUT_PULLUP);
	
	TCNT1 = 0;
	TIMSK1 |= (1<<OCIE1A);
	TCCR1A = 0;
	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10); // CTC, pre-scaler 1024
	OCR1A = (15624); 
	
	init_shift_reg(SHIFT_CLOCK_PIN, SHIFT_DATA_PIN, SHIFT_LATCH_PIN);
	
	displayDigits(Numbers, ORDER); // call function to display numbers on 7 segs
	
	pinMode(BuzzerPin, OUTPUT);
	
	// LCD init
	LCD lcd;
	
	//lcd_init(lcd, );
	
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
		snprintf(lap_records[lap_index], sizeof(lap_records[lap_index]), "LAP%d|%d%d:%d%d:%d%d:%d%d", 
		lap_index++, 
		Numbers[0], Numbers[1], 
		Numbers[2], Numbers[3], 
		Numbers[4], Numbers[5], 
		Numbers[6], Numbers[7]); // I tried being clever, if not working update lap index on its own after
		
		// TODO: Update the LCD with lap string
	}
}

ISR(INT1_vect){ // Stop/Reset Button
	if (Direction != 0){//Stop
		Direction = 0;
	}
	else{ // Reset
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