/*
* USART.c
*
* Created: 12.04.2019 23:30:01
*  Author: Dietrich
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define BAUDRATE 51

void USART_Init(unsigned int baud)  {
	
	//51 ist 9600 Baud
	UBRRH = (unsigned char) (baud >> 8); /* Set baud rate */
	UBRRL = (unsigned char) baud;

	UCSRB = (1 << RXEN) | (1 << TXEN) | 1<< RXCIE | (1 << TXCIE); /* Enable rx and tx */
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0); /* Set frame
	format: 8data, 1stop bit, parity none (initial value) */
	} /* URSEL=1: USCRC, URSEL = 0: UBRRH (initial value) */


	void USART_Transmit(char* message) {

		for (int i = 0; message[i] != '\0'; ++i) {
			/* Wait for empty transmit buffer
			(URDE Date Register Empty Bit ist SET inside the UCSRA Register)*/
			while (!(UCSRA & (1 << UDRE)));

			/* When data is written to the UDR transmit buffer, and the Transmitter
			is enabled, the Transmitter will load the data into the transmit
			Shift Register when the Shift Register is empty. Then the data
			will be serially	transmitted on the TxD pin.
			UDRE wird vom Controller automatisch geCleared*/
			UDR = message[i];
		}
	}

	unsigned char USART_Receive( void )
	{
		/* Wait for data to be received
		RXC flag bit is set when there are unread data in the receive buffer and
		cleared when the receive buffer is empty (i.e., does not contain any unread data).
		If the receiver is disabled, the receive buffer will be flushed and consequently
		the RXC bit will become zero. The RXC Flag can be
		used to generate a Receive Complete interrupt (see description of the RXCIE bit).*/
		while ( !(UCSRA & (1<<RXC)) )
		;
		/* Get and return received data from buffer */
		return UDR;
	}

	

