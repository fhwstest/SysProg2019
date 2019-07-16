/*
 * uebung9_blueservo.c
 *
 * Created: 15.07.2019 14:50:13
 * Author : PC
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "../uebung5_USART/USART.h"


char rec;

void initPWM()
{
	DDRD |= (1<<PD5);	/* Make OC1A pin as output */
	TCNT1 = 0;		/* Set timer1 count zero */
	ICR1 = 2499;	// limit of counter for fast PWM

	// Set fast PWM
	TCCR1A = (1<<WGM11)|(1<<COM1A1);
	// prescaler 64
	TCCR1B = (1<<WGM12)|(1<<WGM13)|(1<<CS10)|(1<<CS11);
}

void moveServo()
{
	// set pulse width dutycycle
	if(rec == 'a'){
		OCR1A = 200;
	}
	
	if(rec == 'b'){
		// 270 max
		OCR1A = 120;
	}
	
	if(rec == 'c'){
		//105 min
		OCR1A = 270;
	}
}

int main(void)
{
	rec = 'a';
	sei();
	USART_Init(9600);
	initPWM();

	while(1)
	{
		moveServo();

	}
}


ISR(USART_RXC_vect){
	rec = USART_Receive();
}

