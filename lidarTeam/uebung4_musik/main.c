/*
 * uebung4_musik.c
 *
 * Created: 15.07.2019 11:17:29
 * Author : PC
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "Note.h"

#define Speaker PORTB
#define SongLen 30

Note Song[SongLen] ={
	{c, viertel},
	{d, viertel},
	{e, viertel},
	{f, viertel},
	{g, halb},
	{g, halb},
	{a, viertel},
	{a, viertel},
	{a, viertel},
	{a, viertel},
	{g, halb},
	{pause, viertel},
	{a, viertel},
	{a, viertel},
	{a, viertel},
	{a, viertel},
	{g, halb},
	{pause, viertel},
	{f, viertel},
	{f, viertel},
	{f, viertel},
	{f, viertel},
	{e, halb},
	{e, halb},
	{d, viertel},
	{d, viertel},
	{d, viertel},
	{d, viertel},
	{c, halb},
	{pause, ganz}
};

void init();

int main(void)
{
	init();
	sei();
    /* Replace with your application code */
    while (1) 
    {
    }
}


void init(){
	DDRB = 0xff;
	
	TIMSK = (1 << OCIE1A)|(1 << OCIE0);
	
	// set timers with prescaler 1024
	TCCR1B |= (1<<CS12)|(1<<CS10)|(1<<WGM12);
	TCCR0 |= (1<<CS02)|(CS00)|(1<<WGM01);
	
	OCR1A = 10;
}


ISR (TIMER0_COMP_vect){
	Speaker = ~Speaker;
}

ISR (TIMER1_COMPA_vect){
	static int Notecounter = 0;
	if(Notecounter == SongLen){
		Notecounter = 0;
	}
	
	// stop/start timer 0 interrupts for pause
	if(Song[Notecounter].freq == pause){
		TIMSK &= ~(1<<OCIE0);
	} 
	else{
		TIMSK |= (1<<OCIE0);
	}
	
	OCR0 = (F_CPU/1024.0)/(double)(Song[Notecounter].freq)*2.0 -1;
	OCR1A = (F_CPU/1024.0)*(double)(Song[Notecounter].len/1000.0)-1;
	
	
	++Notecounter;
}