#include "uart.h"

// ********************************************************************************
// Functions
// ********************************************************************************
void usart_init(void) {
	stdout = &mystdout;
 UBRRH = (unsigned char)(UBRR_VAL>>8);         //Set baud rate, set UBRRH first
 UBRRL = (unsigned char)UBRR_VAL;
 UCSRB = (1<<RXEN) | (1<<TXEN);                //Enabling RX & TX
 UCSRC = (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0); //Set frame format: 8data, 1stop bit, parity none
}

void usart_putchar(unsigned char data) {
  while (!(UCSRA & (1<<UDRE))) /*Wait for empty transmit buffer */
     ;
  UDR = data;                  /*Put data into buffer, sends the data */
}

//This function is called by printf as a stream handler
int usart_putchar_printf(char var, FILE *stream) {
   usart_putchar(var);
   return 0;
}

