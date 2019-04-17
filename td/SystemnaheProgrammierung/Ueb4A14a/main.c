/*
* Ueb4A14a.c
*
* Created: 31.03.2019 18:22:15
* Author : Dietrich
* Topic: Tonleiter
* 
* Pins: PB3 mit Lautsprecher + 
*       GND mit Lautsprecher -  
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//funktionsprototypen (deklaration)
void init();
void settimer();
unsigned int hz2ocr(unsigned int hz);
void playTone(unsigned int hz, unsigned int dauer_ms);
void stoptimer();
void starttimer();

//zählt OCR Matches (bei cHz ist 256 == 1 Sekunde, bei aHz ist 440 == 1 Sekunde!)
volatile unsigned int  isr_ocr0_matches = 0;

static const unsigned int cHz = 256;
static const unsigned int dHz = 294;
static const unsigned int eHz = 330;
static const unsigned int fHz = 349;
static const unsigned int gHz = 392;
static const unsigned int aHz = 440;
static const unsigned int hHz = 494;

int main(void)
{
	unsigned int noten[8] = {cHz, dHz, eHz, fHz, gHz, aHz, hHz};
	unsigned int dauer[8] = {400,400,400,400,400,400,400};
	unsigned int curTone = 1;
	unsigned int arrsize = sizeof(noten) / sizeof(unsigned int);

	init();
		
	while (1)
	{
		playTone(noten[curTone-1], dauer[curTone-1]);
		
		//in Dauerschleife
		if (curTone%arrsize == 0)
		{
			curTone = 1;
		}
		else
		{
			curTone++;
		}
	}
}

void playTone(unsigned int hz, unsigned int dauer_ms)
{
	uint32_t stop_ocr0_matches = 0;
	
	stoptimer();

	//Hertz umrechnen und ins Timer0 OutCompareRegiter 
	OCR0 = hz2ocr(hz);
	
	isr_ocr0_matches = 0;
	
	// hz == (ISR Aufrufe pro Sekunde * spieldauer in ms) / 1000 ms
	stop_ocr0_matches = ((uint32_t) hz * (uint32_t) dauer_ms) / 1000;

	starttimer();

	while (isr_ocr0_matches <= stop_ocr0_matches)
	{

	}

	// ton aus
	//OCR0 = 0;
	stoptimer();
}

unsigned int hz2ocr(unsigned int hz)
{
	// MCU / (Prescaler * Hz)
	// 8.000.000 Hz / (256 *  Hz)
	return 8000000 / (256 * hz) -1;
}

void starttimer()
{
	// Prescaler 256
	//TCCR0 |= (1 << CS00);
	//TCCR0 |= (1 << CS01);
	TCCR0 |= (1 << CS02);
	return;
}

void stoptimer()
{
	TCCR0 &= ~(1 << CS00);
	TCCR0 &= ~(1 << CS01);
	TCCR0 &= ~(1 << CS02);
	return;
}


void init()
{
	settimer();

	//Set I/0 Port config
	DDRA = 0xFF;
	PORTA = 0xFE;
		
	//OC0 Pin als Output
	DDRB = (1<<PB3);
	
}

void settimer()
{
	// Output Compare Match Interrupt erlauben
	TIMSK |= (1 << OCIE0);
	//TIMSK |= (1 << TOIE0);

	// Timer 0 konfigurieren
	//TCCR0 = 0x00 ;        // Reset auf 0
	
	//ComparMatchOutput. Toggle OC0 (I/0 Port) an. (ist der PB3)
	TCCR0 |= (1 << COM00);
	//TCCR0 |= (1 << COM01);

	//CTC Betriebsmodus
	//TCCR0 = (0 << WGM00);
	TCCR0 |= (1 << WGM01);
	
	//wird erst mal nicht gebraucht
	stoptimer();

	OCR0 = 0;

	// Global Interrupts aktivieren
	sei();
}


//OCIE0 Interruptroutine
ISR(TIMER0_COMP_vect) {
	++isr_ocr0_matches;
}

