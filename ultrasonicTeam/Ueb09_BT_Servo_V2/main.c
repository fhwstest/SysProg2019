 /*
 * bluetooth.c
 *
 * Created: 03.06.2019 10:54:34
 * Author : Philipp
 */ 

#define BAUD 9600UL			// Baudrate
#define F_CPU 8000000UL		// CPU Takt

#include <avr/io.h>
#include <util/delay.h>
#include "temperatur.h"


void USART_Init(unsigned intbaud) {
	UBRRH = intbaud >> 8; /* Set baudrate */
	UBRRL = intbaud & 0xFF;
	UCSRB = (1<<RXEN)|(1<<TXEN); /* Enablerxandtx*/
	UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0); /* Set frameformat: 8data, 1stop bit, paritynone(initial value) */
	} /* URSEL=1: USCRC, URSEL = 0: UBRRH (initial value) */
	
void UART_TxChar(char ch)
{
	while (! (UCSRA & (1<<UDRE)));  /* Wait for empty transmit buffer */
	UDR = ch ;
}

unsigned char UART_RxChar()
{
	while ((UCSRA & (1 << RXC)) == 0);/* Wait till data is received */
	return(UDR);		/* Return the byte */
}

void UART_SendString(char *str)
{
	unsigned char j=0;
	
	while (str[j]!=0)		/* Send string till null */
	{
		UART_TxChar(str[j]);
		j++;
	}
}


int main(void)
{
  
  DDRB = 0xFF;
  DDRA = 0xFF;
  PORTB = 0xFF;	//LEDs
  PORTA = 0x00;	//UART - BT Modul
  PORTB &= ~(1<<PB0);	//LED 0 an
  char c;
  USART_Init(51);	//51 -> BAUD Rate auf 9600
  
  UART_SendString("AT\r\n");
  PORTB &= ~(1<<PB1);	//LED 1 an
  
  while(1){
	  c=UART_RxChar(); //recieve von BT Modul
	  UART_TxChar(c);	//sende über Hterm---for Debug
	  //LEDs
	  if(c == 'O') PORTB &= ~(1<<PB2);	//LED an
	  if(c == 'K') PORTB |= (1<<PB2);	//LED aus
	 
	  //Servo
	  for(int i=0; i<10; i++) {//For Schleife weil der Servo das Signal öfters benötigt
		  if(c == '1') {
			   PORTA |= (1<<PA2);
			   _delay_us(1500); //1,5ms entsprechen 90° Drehung
			   PORTA &= ~(1<<PA2);
			   _delay_us(18500);
			 }
	  
		  if(c == '2') {
				PORTA |= (1<<PA2);
				_delay_us(2000); //2ms ensptrechen 180° Drehung
				PORTA &= ~(1<<PA2);
				_delay_us(18000);
			  }
		  if(c == '3') {
			  PORTA |= (1<<PA2);
			  _delay_us(1000); //1ms entsprechen 0° Drehung
			  PORTA &= ~(1<<PA2);
			  _delay_us(19000);
		  }
	  }
	  
  }
}

