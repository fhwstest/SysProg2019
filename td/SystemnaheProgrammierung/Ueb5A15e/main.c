/*
* Ueb5A15e.c
*
* Created: 01.04.2019 23:05:26
* Author : Dietrich
*
* Topic: UART Transfer. Empfangen von Dezimalzahlen übers HTerm und mit den LEDS binär anzeigen
* STK500 Pins: PD0 mit RXD und PD1 mit TXT RS232Spare verbinden
*              PORTB mit LEDs
*
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usart.h"

#define BAUDRATE 51

int main() {
	
	unsigned char recChar = '\0'	;
	char sendbuffer[3] = {'\0','\r','\n'};
	
	//I/0 Pins vom B-Register mit LEDs verbunden
	DDRB = 0xFF;
	PORTB = 0xFF;
	
	//USART Settings einstellen
	USART_Init(BAUDRATE);
		
	//send
	while (1) {
		
		USART_Transmit("Please send decimal number\r\n");
		
		//Warten auf Dezimalzahl (Type im HTERM auf DEC setzen und eine Zahl <256)
		recChar = USART_Receive();
				
		//Empfangene Zahl binär anzeigen
		PORTB = ~recChar;
	}


	return 0;
}


