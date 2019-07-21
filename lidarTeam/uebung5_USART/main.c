/*
 * main.c
 *
 * Created: 15.07.2019 12:45:28
 *  Author: PC
 */

#include <avr/interrupt.h>
#include "USART.h"

#define LED PORTC
#define BUTTON PINA

void init();

int main(void)
{
	init();

	sei();

	USART_Init(9600);

	while (1)
	{
		USART_Transmit(BUTTON);
	}
}

void init()
{
	DDRC = 0xff;
	DDRA = 0x00;
	LED = 0xff;
}

ISR(USART_RXC_vect)
{
	LED = USART_Receive();
}
