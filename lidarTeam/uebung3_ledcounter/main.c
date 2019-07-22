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
void checkButton(uint8_t *counter);

int main(void)
{
	initPorts();
	uint8_t counter = 0;
	while (1)
	{
		checkButton(&counter);
		LED = ~counter;
	}
}

void initPorts()
{
	DDRB = 0xFF;
	LED = 0xFF;

	DDRC = 0x00;
}

void checkButton(uint8_t *counter)
{
	static bool isPressed = false;
	if ((BUTTON != 0xFF) && !isPressed)
	{
		++*counter;
		isPressed = true;
		_delay_ms(100);
	}
	else if (BUTTON == 0xFF)
	{
		isPressed = false;
		_delay_ms(100);
	}
}
