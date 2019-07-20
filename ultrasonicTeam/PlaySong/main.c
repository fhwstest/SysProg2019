/*
 * PlaySong.c
 *
 * Created: 11.07.2019 13:05:56
 * Author : Adrian Laug
 */ 

#define F_CPU 8000000;

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Tones.h"

#define LED_PORT PORTC
#define LED_DDR DDRC
#define SPEAKER_PORT PORTB
#define SPEAKER_DDR DDRB

volatile unsigned int milliseconds = 0;
volatile unsigned int seconds = 0;
volatile unsigned int ovfFlag = 0;
volatile int counter = 0;

void init_timer0(void)
{
	// Timer 0 konfigurieren
	TCCR0 = (1<<WGM01); // CTC Modus
	TCCR0 |= (1<<CS01); // Prescaler 64
	TCCR0 |= (1<<CS00);
	// ((8000000/64)/1000) = 125
	OCR0 = 125-1;

	// Compare Interrupt erlauben
	TIMSK |= (1<<OCIE0);

	// Global Interrupts aktivieren
	sei();
}

void init_timer2(void)
{
	// Timer 0 konfigurieren
	TCCR2 = (1<<WGM21); // CTC Modus
	
	TCCR2 |= (1<<CS22); // Prescaler 256
	
	// ((8000000/64)/1000) = 125
	OCR2 = 118;

	// Compare Interrupt erlauben
	TIMSK |= (1<<OCIE2);

	// Global Interrupts aktivieren
	sei();
}

int main(void)
{
	LED_DDR = 0xff;
	LED_PORT = 0xff;
	SPEAKER_DDR = 0xff;
	SPEAKER_PORT = 0xff;
	
	init_timer0();
	init_timer2();
	int curSec = seconds;
	int enten[54];
	alleEnten(enten);
	for (int i = 0; i <= 52;)
	{
		makePause();
		playTone(enten[i+1], enten[i]);
		i = i + 2;
	}
	
	SPEAKER_DDR = 0x0;
	while(1)
	{
		
		/*
		LED_PORT = ~seconds;
		getNextTone();
		if (curSec != seconds) {
			makePause();
			curSec = seconds;
		}
		*/
	}
}

void makePause(void)
{
	cli();
	milliseconds = 0;
	sei();
	while(milliseconds <= PAUSE)
		SPEAKER_DDR = 0x00;
	SPEAKER_DDR = 0xff;
}

void alleEnten(int* enten)
{
	enten[0] = NORMAL_TONE;
	enten[1] = TONE_C;
	enten[2] = NORMAL_TONE;
	enten[3] = TONE_D;
	enten[4] = NORMAL_TONE;
	enten[5] = TONE_E;
	enten[6] = NORMAL_TONE;
	enten[7] = TONE_F;
	enten[8] = LONG_TONE;
	enten[9] = TONE_G;
	enten[10] = LONG_TONE;
	enten[11] = TONE_G;
	enten[12] = NORMAL_TONE;
	enten[13] = TONE_A;
	enten[14] = NORMAL_TONE;
	enten[15] = TONE_A;
	enten[16] = NORMAL_TONE;
	enten[17] = TONE_A;
	enten[18] = NORMAL_TONE;
	enten[19] = TONE_A;
	enten[20] = LONG_DONG_TONE;
	enten[21] = TONE_G;
	enten[22] = NORMAL_TONE;
	enten[23] = TONE_A;
	enten[24] = NORMAL_TONE;
	enten[25] = TONE_A;
	enten[26] = NORMAL_TONE;
	enten[27] = TONE_A;
	enten[28] = NORMAL_TONE;
	enten[29] = TONE_A;
	enten[30] = LONG_DONG_TONE;
	enten[31] = TONE_G;
	enten[32] = NORMAL_TONE;
	enten[33] = TONE_F;
	enten[34] = NORMAL_TONE;
	enten[35] = TONE_F;
	enten[36] = NORMAL_TONE;
	enten[37] = TONE_F;
	enten[38] = NORMAL_TONE;
	enten[39] = TONE_F;
	enten[40] = LONG_TONE;
	enten[41] = TONE_E;
	enten[42] = LONG_TONE;
	enten[43] = TONE_E;
	enten[44] = NORMAL_TONE;
	enten[45] = TONE_D;
	enten[46] = NORMAL_TONE;
	enten[47] = TONE_D;
	enten[48] = NORMAL_TONE;
	enten[49] = TONE_D;
	enten[50] = NORMAL_TONE;
	enten[51] = TONE_D;
	enten[52] = LONG_DONG_TONE;
	enten[53] = TONE_C;	
}


void playTone(int tone, int length)
{
	cli();
	milliseconds = 0;
	OCR2 = tone;
	sei();
	while(milliseconds <= length);
}

void getNextTone()
{
	switch(seconds % 8)
	{
	case 0:
		//makePause();
		OCR2 = TONE_C;
		break;
	case 1:
		//makePause();
		OCR2 = TONE_D;
		break;
	case 2:
		//makePause();
		OCR2 = TONE_E;
		break;
	case 3:
		//makePause();
		OCR2 = TONE_F;
		break;
	case 4:
		//makePause();
		OCR2 = TONE_G;
		break;
	case 5:
		//makePause();
		OCR2 = TONE_A;
		break;
	case 6:
		//makePause();
		OCR2 = TONE_H;
		break;
	case 7:
		//makePause();
		OCR2 = TONE_C2;
		break;
	}
}

/*
Der Compare Interrupt Handler 
wird aufgerufen, wenn 
TCNT1 = OCR1A = 125-1 
ist (125 Schritte), d.h. genau alle 1 ms
*/
ISR (TIMER0_COMP_vect)
{
	if (milliseconds >= 500)
	{
		seconds++;
		milliseconds = 0;
	}
	milliseconds++;
}

ISR (TIMER2_COMP_vect)
{
	SPEAKER_PORT = ~SPEAKER_PORT;
}