/*
 * USART.c
 *
 * Created: 29.04.2019 12:16:48
 *  Author: Paul
 */ 

#include <avr/io.h>
#include "USART.h"

void USART_Init(unsigned int baud){
	UBRRH = (unsigned char)(baud>>8);
	UBRRL = (unsigned char)baud;
	UCSRB = (1<<RXEN)|(1<<TXEN)|(1<<RXC);
	UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
}

unsigned char USART_Receive(void){
	while (!(UCSRA &(1<<RXC))){
		
	}
	return UDR;
}

void USART_Transmit(unsigned char msg){
	while(!(UCSRA &(1<<UDRE))){
		
	}
	UDR = msg;
}
