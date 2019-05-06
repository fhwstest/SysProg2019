/*
 * UART.c
 *
 * Created: 08.04.2019 12:44:39
 * Author : Paul
 */ 
#include <avr/interrupt.h>
#include "USART.h"

int main(void)
{
	DDRC = 0xff;
	DDRA = 0x00;
	PORTC = 0xff;
	
	sei();
    /* Replace with your application code */
	USART_Init(51);
	
    while (1) 
    {
		USART_Transmit(PINA);
    }
}

ISR(USART_RXC_vect){
	PORTC = USART_Receive();
}


