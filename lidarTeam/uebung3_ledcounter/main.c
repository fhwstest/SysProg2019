/*
 * Uebungen.c
 *
 * Created: 15.07.2019 10:20:59
 * Author : PC
 */ 

#include <avr/io.h>
#include <avr/delay.h>

#define LED PORTB
#define BUTTON PINC

void initPorts();
void checkButton(bool *isPressed, uint8_t *counter);

int main(void)
{
	initPorts();
	uint8_t counter = 0;
	bool isPressed = false;
    /* Replace with your application code */
    while (1) 
    {
		checkButton(&isPressed, &counter);
		LED = ~counter;
    }
}

void initPorts(){
	DDRB = 0xFF;
	LED = 0xff;
	
	DDRC = 0x00;
}

void checkButton(bool *isPressed, uint8_t *counter)
{
	if((BUTTON != 1) && !*isPressed){
		++*counter;
		*isPressed = true;
		_delay_ms(100);
	}
	else if (BUTTON == 1){
		*isPressed = false;
		_delay_ms(100);
	}
}

