/*
* Main.c
*
* Created: 20.06.2019 11:36:23
* Author : Torsten Dietrich
* Servor über Bluetooth HC05 Modul steuern
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "print.h"

#define BAUDRATE 51 //9600 Baud => 8 MHz / (16 * (51+1)) ~ 9600 Baud

//Function Prototypes
void Init_Servo_PWM(void);
void USART_Init(void);
void USART_Transmit(unsigned char);
void turn_0(void);
void turn_180(void);
void turn_90(void);

//*******************************************************************
int main(void) {

    sei(); //enable global interrupts for USART-Receive-ISR

    initPrintf();     //initialize printing
    USART_Init();     //initialize USART
    Init_Servo_PWM(); //initialize Servo
    while (1)         //wait for commands
    {
        //do nothing
    }
}

//*******************************************************************
void Init_Servo_PWM()
{
    DDRD = 0x30;                                 // Set Port D4 (OC1B) and D5 (OC1A) as Output. OC1n als IRQ PINs. 

    // Set up the two Control registers of Timer1. 
    TCCR1A = (1<<WGM10)|(1<<COM1A1)|(1<<COM1B1); // (WGM10, WBM12)   = WaveFormGeneration is Fast PWM 8 Bit (TOP 0xFF),  
    TCCR1B = (1<<WGM12)                          // (COM1A1, COM1B1) = COmparMatch OC1A and OC1B are cleared on compare match and set at BOTTOM.
             |(1<<CS12) |(1<<CS10);              // (CS12, CS10)     = ClockSelect Prescaler is 1024.
                                                 // Ein Takt: 32,78 ms 
                                                 // 8MHz: 8000000/1024/256 =  30,51 
                                                 // 1000ms / 30,51         =  32,78 ms/Takt

    OCR1A = 255;                                 //Init OutputCompareRegister. 
}


//*******************************************************************
void USART_Init() {
    
	UBRRH = (unsigned char) (BAUDRATE >> 8); 
	UBRRL = (unsigned char) BAUDRATE;
	UCSRB |= (1 << RXEN) | (1 << TXEN);    // enable receiver and transmitter
	UCSRB |= (1 << RXCIE) | (1 << TXCIE) | (1 << UDRIE);    //enable receive and transmit interrupts
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0); 	//Kein OR um Initalwerte zu halten: Set frame format: 8 data, 1stop bit, parity none (initial value) */
	}

//*******************************************************************
void USART_Transmit(unsigned char data) {
	
    /* Wait for empty transmit buffer */
	while (!(UCSRA & (1 << UDRE)));
	/* Put data into buffer, sends the data */
	UDR = data;
}

//*******************************************************************
void turn_0() {
    printf("turn0\r\n");
	OCR1A = 4;              // (4*32780us)/256 = 512 us = 0,512 ms
}
void turn_90() {
    printf("turn90\r\n");
	OCR1A = 8;              // (8*32780us)/256 = 1024 us = 1,024 ms  
}
void turn_180() {
    printf("turn180\r\n");   
	OCR1A = 19;             // (19*32780us)/256 = 2432 us = 2,432 ms   
} 

//*******************************************************************
ISR(USART_RXC_vect) {
       
    printf("USART RECEIVE\n");
    unsigned char c = UDR;

    if(c == '3')
    turn_0();
    if(c == '2')
    turn_90();
    if(c=='1')
    turn_180();
    printf("%x",c);
}

