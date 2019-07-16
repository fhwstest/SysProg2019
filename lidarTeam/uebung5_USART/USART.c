/*
 * uebung5_USART.c
 *
 * Created: 15.07.2019 12:34:02
 * Author : PC
 */ 
#include <avr/io.h>
#include "USART.h"

void USART(unsigned int baud){
	UBRRH = (unsigned char)(baud>>8);
	UBRRL = (unsigned char)baud;
	UCSRB = (1<<RXEN)|(1<<TXEN)|(1<<RXCIE);
	
	// daten und stoppbits (Messageaufbau) setzen 8xdata 1xstop
	UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
}

void USART_Init(unsigned int baud){
	USART((int)((F_CPU/16.0*baud)-1))
}

unsigned char USART_Receive(void){
	// wait for receive complete
	while (!(UCSRA &(1<<RXC))){
		
	}
	return UDR;
}

void USART_Transmit(unsigned char msg){
	// wait for transmit complete (empty buffer)
	while(!(UCSRA &(1<<UDRE))){
		
	}
	UDR = msg;
}


void USART_TransmitStr(const char* string){
	for(int i = 0, string[i]!='\0', i++){
		USART_Transmit(string[i]);
	}
}
