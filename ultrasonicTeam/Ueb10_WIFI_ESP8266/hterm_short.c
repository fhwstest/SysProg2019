/*****************************************************************************
*Implements printf() using UART on ATmega32
*Connect STK500 via serial cable with PC
*Connect PD0=RX and PD1=TX on STK500s
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

//Function Prototypes
void          usart_init(void);
void          usart_putchar(unsigned char );
int           usart_putchar_printf(char , FILE *); 
//unsigned char usart_getchar(void);
//void          usart_pstr(char * );

// Global Variables for printf
static FILE mystdout = FDEV_SETUP_STREAM(usart_putchar_printf, NULL, _FDEV_SETUP_WRITE);

// ********************************************************************************
// main(), just for standalone
// ********************************************************************************
/* int main(void) {
 uint8_t test_value = 64;

 stdout = &mystdout; // setup stdio stream
 usart_init();
 while (1)
 {
   printf("Here is myvalue in hex: 0x%X\n", test_value);
    _delay_ms(1000);
 }
}   */

// ********************************************************************************
// Functions
// ********************************************************************************
void usart_init(void) {
  
 UCSRC = (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0); //Set frame format: 8data, 1stop bit, parity none

 UBRRH = (unsigned char)(UBRR_VAL>>8);  // Set baud rate (9600), set UBRRH first
 UBRRL = (unsigned char)UBRR_VAL;
 UCSRB |= (1 << RXEN) | (1 << TXEN);    // enable receiver and transmitter
 UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 <<  UCSZ0); //Hier ist extra kein .OR. um Initalwerte zu halten: ...
                                                      //...Set frame format: 8 data, 1stop bit, parity none (initial value)
 
 UCSRB |= (1 << RXCIE) ;                // enable receive interrupts
 /* Im Default bereits auf 0
 UCSRB &= ~(1 << TXCIE);                // disable transmit interrupt
 UCSRB &= ~(1 << UDRIE);                // disable data-register-empty interrupt
 */ 
 
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
// ********************************************************************************
// Further functions
// ********************************************************************************
unsigned char usart_getchar(void){
  while (!(UCSRA & (1<<RXC))) // Wait for data to be received
     ;
  return UDR;    // Get and return received data from buffer
} 

/* void usart_pstr(char *s) {
    while (*s)     {
        usart_putchar(*s);
        s++;
    }
} */
