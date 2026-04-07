
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define LOW 0
#define HIGH 1
#define LSBFIRST 0
#define MSBFIRST 1

void pinMode(unsigned char pin, unsigned char mode);
void digitalWrite(unsigned char pin, unsigned char value);
unsigned char digitalRead(unsigned char pin);

void shiftOut(unsigned char dataPin,
	unsigned char clockPin,
	unsigned char bitOrder,
	unsigned char value
	);
	
void togglePin(unsigned char pin);
