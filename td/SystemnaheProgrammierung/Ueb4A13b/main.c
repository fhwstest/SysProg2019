/*
	Author: Torsten Dietrich
	Date: 2019-04-04
	Topic: BIN Counter
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//zählt Millisekunden
volatile unsigned int  milliseccnt = 0;
volatile unsigned int portastate  = 1;

int a13a(void);

int main(void)
{
	a13a();
	return 0;
}

int a13a(void) {

    int seconds = 0;

	// Output Compare Match Interrupt erlauben
	TIMSK |= (1 << OCIE0);
	TIMSK |= (1 << TOIE0);

	// Timer 0 konfigurieren
	TCCR0 = 0x00 ;        // Reset auf 0

	// Prescaler 64
	TCCR0 |= (1 << CS00);
	TCCR0 |= (1 << CS01);
	//TCCR0 |= (1 << CS02);

	//ComparMatchOutput (I/0 Port) aus
	//TCCR0 |= (0 << COM00);
	//TCCR0 |= (0 << COM01);

	//CTC Betriebsmodus
	//TCCR0 = (0 << WGM00);
	TCCR0 |= (1 << WGM01);

	
	// 8.000.000 Hz / (64 * 1000 Hz) = 125
	OCR0 = 125-1; // frequenz OCR 1000Hz

	// Global Interrupts aktivieren
	sei();

	DDRA = 0xFF;
	PORTA = 0x01;
	
	
	while(1) {
		if (milliseccnt >= 1000) {
			
			seconds++;
			PORTA = ~seconds;
			milliseccnt = 0;
		}
	}

	return 0;

}

//OCIE0 Interruptroutine
ISR(TIMER0_COMP_vect) {
	
	++milliseccnt;


}
