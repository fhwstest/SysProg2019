/*****************************************************************************
*Implements print_xxxx() using UART on ATmega32
*Connect STK500 via serial cable with PC
*connect PD0=RX and PD1=TX on STK500s
*Here: Busy waiting
*http://www.mikrocontroller.net/articles/AVR-GCC-Tutorial/Der_UART 
*
*Based on http://efundies.com/avr-and-printf/
*******************************************************************************/
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h> 

//#define F_CPU 8000000UL
#define BAUD     9600UL
#define UBRR_VAL (F_CPU/(BAUD*16)-1) //UCSRA:U2X == 0, else (BAUD*8), Asynchr. Normal Mode

// Function Prototypes
void          usart_init(void);
void          usart_putchar(unsigned char );
int           usart_putchar_printf(char , FILE *); 
//unsigned char usart_getchar(void);
//void          usart_pstr(char * );

// Global Variables for printf
static FILE mystdout = FDEV_SETUP_STREAM(usart_putchar_printf, NULL, _FDEV_SETUP_WRITE);
