/*
 * uebung7_EEPROM.c
 *
 * Created: 15.07.2019 13:52:49
 * Author : PC
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <UART.h>
#include <stdio.h>

#define I2C_SCK 400000
#define SLA_R 0xA1
#define SLA_W 0xA0

#define TW_START 0x8
#define TW_SLAW_ACK 0x18
#define TW_SLAR_ACK 0x40
#define TW_WRITE_ACK 0x28

void init();
void start();
void stop();

void writeData(uint8_t data, uint8_t successState, const char *msg);
uint8_t readData();

void printErg(const char *msg, uint8_t code, uint8_t expectedCode);

void writeDataToMem(const int data, const int address);

void readDataFromMem(const int address);

int main()
{
	const int address = 0;
	const int data = 0x0b;

	init();

	writeDataToMem(data, address);

	_delay_ms(200);

	readDataFromMem(address);
}

void writeDataToMem(const int data, const int address)
{
	start();

	//write data
	//32 pages with 16 bytes
	//5 bit page address, 4 bit for byte in page
	writeData(SLA_W, TW_SLAW_ACK, "Set in Write Mode");
	writeData(address, TW_WRITE_ACK, "Set Address");
	writeData(data, TW_WRITE_ACK, "Write Data");

	stop();
}

void readDataFromMem(const int address)
{
	start();

	writeData(SLA_W, TW_SLAW_ACK, "Set in write Mode 2");
	writeData(address, TW_WRITE_ACK, "Set read Address");

	// no stop so master cant be aquired by another participant
	start();

	writeData(SLA_R, TW_SLAR_ACK, "Set in read Mode");
	readData();

	stop();
}

void init()
{
	//TWPS1 und TWPS0 = 0, Prescaler 1
	TWSR = 0x00;
	TWBR = (F_CPU / I2C_SCK - 16); //I2C_SCK ist die gewünschte Frequenz
}

void start()
{
	//Send START condition
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

	//Wait for TWINT Flag set. This indicates that the START condition has been transmitted
	while ((TWCR & (1 << TWINT)) == 0)
		;
}

void stop()
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void writeData(uint8_t data, uint8_t successState, const char *msg)
{
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN); //Clear TWINT bit in TWCR to start transmission
	while (!(TWCR & (1 << TWINT)))
		;
	// return code of command: uint8_t erg =TWSR & 0xF8;
}

uint8_t readData()
{
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
		;

	uint8_t data = TWDR;

	return data;
}
