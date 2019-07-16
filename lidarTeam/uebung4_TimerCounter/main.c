/*
 * uebung4_TimerCounter.c
 *
 * Created: 15.07.2019 10:41:57
 * Author : PC
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define LED PORTB

void init();
void toggleLED();
void binaryCounter();
void BCDCounter();

int main(void)
{
	// initialize LEDS and counter
	init();
	
	// enable interrupts
	sei();
	
    /* Replace with your application code */
    while (1) 
    {
    }
}


void init(){
	DDRB = 0xff;
	LED = 0xff;
	
	// Enable Timer 1 compare interrupt
	TIMSK |= (1<<OCIE1A);
	
	// set compare register
	OCR1A = ((F_CPU/1024*1) - 1);
	
	// set timer with prescaler 1024 and reset timer on compare match
	TCCR1B |= (1<<CS12)|(1<<CS10)|(1<<WGM12);
}


void toggleLED(){
	if (LED == 0xfe){
		LED = 0xfd;
	}
	if (LED == 0xfd){
		LED = 0xfe;
	}
}


void binaryCounter(){
	static uint8_t counter = 0;
	LED = ~counter++; 
}


void BCDCounter(){
	static int counter = 0;
	
	uint8_t left = counter/10;
	uint8_t right = counter%10;
	
	uint8_t setLED = left<<4 | right;
	
	LED = ~ setLED;	
}


ISR (TIMER1_COMPA_vect){
	toggleLED();
}

