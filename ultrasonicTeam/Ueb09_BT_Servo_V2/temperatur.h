/*
 * OneWire.c
 *
 * Created: 13.05.2019 12:03:54
 * Author : Philipp
 */ 
#ifndef temp
#define temp

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define delay_A 6
#define delay_B 64
#define delay_C 60
#define delay_D 10
#define delay_E 9
#define delay_F 55
#define delay_G 0
#define delay_H 480
#define delay_I 70
#define delay_J 410
#define DataPin PC0

#define READ_ROM 0x33
#define SKIP_ROM 0xCC
#define READ_SCRATCHPAD 0xBE
#define MEASURE 0x44

void ow_write_bit(char byte) {
	cli();
	if (byte == 0) {
		DDRC = 0xFF; //setze Port C auf Ausgang
		PORTC |= (1 << DataPin);
		PORTC &= ~(1 << DataPin);
		_delay_us(delay_C);
		DDRC = 0x00; //setze Port C auf Eingang, wird automatisch high
		_delay_us(delay_D);
		printf("write 0\n");
	}
	
	if (byte == 1) {
		DDRC = 0xFF; //setze Port C auf Ausgang
		PORTC |= (1 << DataPin);
		PORTC &= ~(1 << DataPin);
		_delay_us(delay_A);
		DDRC = 0x00; //setze Port C auf Eingang, wird automatisch high
		_delay_us(delay_B);
		printf("write 1\n");
	}
	sei();
}

void ow_write_byte(unsigned char byte) {
		for (int i = 0; i < 8; i++)
		{
			ow_write_bit(byte & 0x01);
			byte >>= 1;
		}
}

unsigned char ow_read_bit(void) {
	cli();
	DDRC = 0xFF; //setze Port C auf Ausgang
	PORTC &= ~(1 << DataPin); //Pin auf Low ziehen
	_delay_us(delay_A);
	DDRC = 0x00; //setze Port C auf Eingang, wird automatisch high
	_delay_us(delay_E);
	unsigned char read = (PINC & (1 << DataPin)); //0 oder 1 vom Slave
	_delay_us(delay_F);
	sei();
	return read;	
}

unsigned char ow_read_byte() {
	unsigned char data = 0;
	
	for (int i = 0; i < 8; i++) {
		data >>= 1;
		unsigned char msb = ow_read_bit();
		data |= (msb << 7);
	}
	return data;
}

unsigned char ow_reset() {
	cli();
	DDRC = 0xFF; //setze Port C auf Ausgang
	PORTC |= (1 << DataPin);
	PORTC &= ~(1 << DataPin);
	_delay_us(delay_H);
	DDRC = 0x00; //setze Port C auf Eingang, wird automatisch high
	_delay_us(delay_I);
	unsigned char presence = (PINC & (1 << DataPin));
	_delay_us(delay_J);
	sei();
	printf("reset\n");
	printf("%d\n", presence);
	return presence;
}


int temp()
{	
	//READ ROM
	unsigned char test = ow_reset();
	ow_write_byte(READ_ROM);
	for(int i = 0; i<8; i++) {
		unsigned char data =ow_read_byte();
		printf("data: 0x%02X \n", data);
	}
	
	//READ TEMP
	unsigned char test2 = ow_reset();
	ow_write_byte(SKIP_ROM);
	ow_write_byte(MEASURE);
	_delay_ms(50);
	test2 = ow_reset();
	ow_write_byte(SKIP_ROM);
	ow_write_byte(READ_SCRATCHPAD);
	printf("RAUMTEMEPRATUR:");
	unsigned char data[9];
	for(int i = 0; i<9; i++) {
		data[i] = ow_read_byte();
	}	
	//printf("%d,%d", data[0]>>1, data[0]&0x01 ? 5 : 0);
	int tem = data[0]>>1;
	return tem;
}

#endif /* temp*/