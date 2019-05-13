/*
 * SPISD.c
 *
 * Created: 29.04.2019 10:20:54
 * Author : Paul
 */ 

#include <avr/delay.h>
#include <avr/io.h>
#include "USART.h"


#define EN1 PD4
#define EN2 PD5
#define SS PB4


unsigned char CMD0[] = {0x40, 0x00, 0x00, 0x00, 0x00, 0x95};
unsigned char CMD1[] = {0x41, 0x00, 0x00, 0x00, 0x00, 0x01};
unsigned char CMD55[] = {0x77, 0x00, 0x00, 0x00, 0x00, 0x01};

unsigned char ACMD41[] = {0x69, 0x00, 0x00, 0x00, 0x00, 0x01};

void spiInit();
void SDInit();
void SPITransmit(unsigned char data);
unsigned char SPIReceive();
unsigned char sdCmmnd(unsigned char* CMD, unsigned int cmd_NR);
void write();
void read();


int main(void)
{
	USART_Init(51);
	USART_Transmit('a');
	spiInit();
	USART_Transmit('b');
	SDInit();
	USART_Transmit('c');
	
    while (1) 
    {
    }
}


void spiInit(){
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);
	SPSR = (1<<SPI2X); 
}


void SDInit(){
	PORTD |= (1 << EN2);
	PORTD &= ~(1 << EN1);
	_delay_ms(75);
	PORTD |= (1 << EN1);
	PORTD &= ~(1 << EN2);
	
	PORTB |= (1 << SS);
	
	for (unsigned int i = 0; i <10; i++){
		SPITransmit(0xFF);
	}
	
	PORTB &= ~(1 << SS);
	
	char received;
	
	do 
	{
		received = sdCmmnd(CMD0, 0);
		USART_Transmit('i');
	} while (received != 0x01);
	
	
	do 
	{
		sdCmmnd(CMD55, 55);
		received = sdCmmnd(ACMD41, 141);
	} while (received != 0x00);
}

void SPITransmit(unsigned char data){
	SPDR = data;
	while(!(SPSR) & (1<<SPIF)){
		;
	}
}

unsigned char SPIReceive(){
	while(!(SPSR) & (1<<SPIF)){
		;
	}
	return SPDR;
}


unsigned char sdCmmnd(unsigned char* CMD, unsigned int cmd_NR){
	SPITransmit(CMD[0]);
	SPITransmit(CMD[1]);
	SPITransmit(CMD[2]);
	SPITransmit(CMD[3]);
	SPITransmit(CMD[4]);
	SPITransmit(CMD[5]);
	
	unsigned char response = 0;
	
	if (cmd_NR == 0){
		do 
		{
			SPITransmit(0xFF);
			response = SPIReceive();
		} while (response != 0x01);
	}
	else{
		do
		{
			SPITransmit(0xFF);
			response = SPIReceive();
		} while (response != 0x00);
	}
	return response;
}


void write(){
	
}

void read(){
	
}