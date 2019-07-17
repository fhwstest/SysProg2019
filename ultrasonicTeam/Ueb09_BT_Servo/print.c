/*
 * print.c
 *
 * Created: 02.05.2018 09:36:28
 *  Author: Felix
 */ 

#define BAUDTRATE 51	// = 8000000 /(16*9600) = Systemtakt/( 16 * Baud)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

int print_initialized = 0;
int printf_initialized = 0;

void initPrint(){
	UBRRH = (unsigned char)(BAUDTRATE>>8);
	UBRRL = (unsigned char)BAUDTRATE;
	UCSRB |= (1<<RXEN)|(1<<TXEN);	// enable receiver and transmitter
	UCSRC = (1<<URSEL)|(1<<USBS)|(1<<UCSZ1)|(1<<UCSZ0); //Set frame format: 8data, 2stop bit
	print_initialized = 1;
}

void print(char* message){
	if (!print_initialized)
		initPrint();
	
	int i = 0;
	while (message[i] != '\0'){
		while (! ( UCSRA & (1<<UDRE)) ) ;	// wait for empty transmit buffer
		UDR = message[i++];
	}
}

int usart_putchar_printf(char data, FILE *stream){
		
	while (! ( UCSRA & (1<<UDRE)) ) ;	// wait for empty transmit buffer
	UDR = data;
	
	return 0;
}


static FILE mystdout = FDEV_SETUP_STREAM(usart_putchar_printf, NULL, _FDEV_SETUP_WRITE);


void initPrintf(){
	
	if (!print_initialized)
		initPrint();
	
	stdout = &mystdout;
	
	printf_initialized = 1;
	
	printf("printf initialized\n");
}

void printBuf(char* str, int* arr, int length){
	for (int i=0; i<length; ++i)
		printf(str, arr[i]);
}