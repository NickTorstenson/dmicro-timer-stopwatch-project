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

#define DELAY 10 //Number of ms between the smallest counts


void incrementDigits (int direction, int index, char digits[]);

void displayDigits(char digits[], char direction);


volatile int STATE = IDLE;
char Numbers[8] = {0,0,0,0, 0,0,0,0};
char ORDER = LSBFIRST;
volatile int Direction = 0; //1 counts up, 0 doesn't count, -1 counts down
volatile char Mode = Up; // Up or Down for counting up or down, defaults to up on startup
volatile char Count = 0;


int main(void)
{
	
	EIMSK |= (1<<INT0); // Set PORTD2 interrupt (Start/Lab)
	EIMSK |= (1<<INT1); // Set PORTD3 interrupt (Stop/Reset)
	EICRA = 0b00001010; // Both are set to trigger on the falling edge
	
	PCICR |= (1<<PCIE0); //sets PC interrupt to look at PORTB
	PCMSK0 |= (1<<PCINT0); //sets PC interrupt to look at Pin0 (PORTB0) (Mode)
	
	sei();
	
	while(1){
		if (STATE == IDLE){
			SLEEP_MODE_IDLE;
		}
		else if (STATE == COUNT_UP){
			displayDigits(Numbers, ORDER); // call function to display numbers on 7 segs
			incrementDigits(Direction, 7, Numbers);  //increments count
			_delay_ms(DELAY);
		}
		else if (STATE == SET_NUMBERS){
			//Set Numbers code
		}
		else if (STATE == COUNT_DOWN){
			displayDigits(Numbers, ORDER); // call function to display numbers on 7 segs
			incrementDigits(Direction, 7, Numbers);  //increments count
			_delay_ms(DELAY);
		}
		else if (STATE == RESET){
			Numbers[0] = 0; //set entire array to zero
			Numbers[1] = 0;
			Numbers[2] = 0;
			Numbers[3] = 0;
			Numbers[4] = 0;
			Numbers[5] = 0;
			Numbers[6] = 0;
			Numbers[7] = 0;
			//Add Idle Timer
		}
		else {
			STATE = IDLE;
		}
	}
}

ISR(INT0_vect){ // Start/Lap Button
	if (STATE == (IDLE|RESET|SET_NUMBERS)){ //Start
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

ISR(PCINT0_vect){ // Mode
	if (Count == 2){
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
