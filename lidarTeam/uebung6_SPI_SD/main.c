/*
 * uebung6_SPI_SD.c
 *
 * Created: 15.07.2019 12:56:35
 * Author : PC
 */

#include <avr/io.h>
#include <avr/delay.h>

#include <stdio.h>
#include "../uebung5_USART/USART.h"

#define MOSI 5
#define SCK 7
#define MISO 6
#define CD 3
#define SS 4
#define EN1 1
#define EN2 2

#define SDCard PORTB
#define BLOCKSIZE 512

const int initCrc = 94;

void initSpi();
void initSdCard();

void reset();

void waitForAnswer(uint8_t expectedAnswer);

void writeByte(uint8_t address, uint8_t value);
void SPI_MasterTransmit(uint8_t cData);

void buildMessage(uint8_t command, uint32_t argument, uint8_t crc, uint8_t *array);
void sendCommand(uint8_t command, uint32_t argument, uint8_t crc);

void setBlockLength();

void printErg(const char *msg);

uint32_t uint32EndianConversion(uint32_t num);

void readBlock(unsigned unit8_t *buff, const uint8_t address);

int main()
{
	USART_Init(9600);

	initSpi();

	initSdCard();
	printErg("Init");

	setBlockLength();
	printErg("Set Block");

	writeByte(1, 1);
	printErg("Write Byte");

	uint8_t data[BLOCKSIZE];
	readBlock(data, 1);

	for (uint8_t i = 0; i < BLOCKSIZE; i++)
	{
		USART_Transmit(data[i]);
	}

	while (1)
		;
}

void waitForAnswer(uint8_t expectedAnswer)
{
	while (SPDR != expectedAnswer)
	{
		SPI_MasterTransmit(0xff);
	}
}

void printErg(const char *msg)
{
	char str[3];
	sprintf(str, "%02X", SPDR);

	USART_TransmitStr(msg);
	USART_TransmitStr(" war ");
	USART_TransmitStr(SPDR == 0 ? "erfolgreich" : "nicht erfolgreich");
	USART_TransmitStr(" Code: ");
	USART_TransmitStr(str);
	USART_TransmitStr("\r\n");
}

void initSpi()
{
	/* Set MOSI and SCK etc. output, all others input */
	DDRB = (1 << MOSI) | (1 << SCK) | (1 << EN1) | (1 << EN2) | (1 << SS);

	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

void SPI_MasterTransmit(uint8_t cData)
{
	/* Start transmission */
	SPDR = cData;
	/* Wait for transmission complete */
	while (!(SPSR & (1 << SPIF)))
		;
}

uint32_t uint32EndianConversion(uint32_t num)
{
	return ((num >> 24) & 0xff) |	// move byte 3 to byte 0
		   ((num << 8) & 0xff0000) | // move byte 1 to byte 2
		   ((num >> 8) & 0xff00) |   // move byte 2 to byte 1
		   ((num << 24) & 0xff000000);
}

void buildMessage(uint8_t command, uint32_t argument, uint8_t crc, uint8_t *array)
{
	// start bit | transmission bit | command index | argument | CRC7 | end bit
	// 0|1|6bit command|32bit argument|7bit CRC|1
	uint8_t *commandAddress = array;
	*commandAddress |= (1 << 6); // Set second bit of byte one to 1
	*commandAddress |= command;  // Set command

	uint32_t *argumentAddress = (uint32_t)(array + 1);
	*argumentAddress = uint32EndianConversion(argument);

	uint8_t *crcAddress = array + 5;
	crc = crc << 1;
	crc |= 1;
	*crcAddress = crc;
}

void sendCommand(uint8_t command, uint32_t argument, uint8_t crc)
{
	uint8_t array[6] = {};
	buildMessage(command, argument, crc, array);

	for (int i = 0; i < 6; i++)
	{
		SPI_MasterTransmit(array[i]);
	}
}

void reset()
{
	SDCard &= ~(1 << EN1);
	SDCard |= (1 << EN2);

	SDCard &= ~(1 << EN2);
	SDCard |= (1 << EN1);

	_delay_ms(60);

	SDCard &= ~(1 << EN1);
	SDCard |= (1 << EN2);
}

void initSdCard()
{
	SDCard &= ~(1 << SS);

	reset();

	SDCard |= (1 << SS);

	// Wait 10 cycles
	for (int i = 0; i < 10; i++)
	{
		SPI_MasterTransmit(0xff);
	}

	// put SD Card in idle SPI Mode
	SDCard &= ~(1 << SS);

	const uint8_t initializeCommand = 0;
	sendCommand(initializeCommand, 0, initCrc);
	waitForAnswer(0x1);

	// only next command after this on is an Application Command
	const uint8_t enableApplicationCommand = 55;
	sendCommand(enableApplicationCommand, 0, 0);
	waitForAnswer(0);

	// switch SD Card from idle to operation mode
	const uint8_t initSequenceCommand = 41;
	sendCommand(initSequenceCommand, 0, 0);
	waitForAnswer(0);
}

void writeByte(uint8_t address, uint8_t value)
{
	// send write command to sd card
	const uint8_t writeBlockCommand = 24;
	sendCommand(writeBlockCommand, address, 0);

	// send  start block token to sd card 254=single block write
	const uint8_t startBlock = 254;
	SPI_MasterTransmit(startBlock);

	// send 512 bytes of data to sd card
	SPI_MasterTransmit(value);
	for (int i = 0; i < (BLOCKSIZE - 1); ++i)
	{
		SPI_MasterTransmit(0);
	}
}

void setBlockLength()
{
	// set sd card memory block length to 512
	const uint8_t setBlockLengthCommand = 16;
	sendCommand(setBlockLengthCommand, BLOCKSIZE, 0);
}

unsigned uint8_t receiveByte()
{
	SPI_MasterTransmit(0xFF);
	while (!(SPSR & (1 << SPIF))) //End of transmission flag
		;
	return SPDR;
}

void readBlock(unsigned unit8_t *buff, const uint8_t address)
{
	const uint8_t readSingleBlockCommand = 17;
	sendCommand(readSingleBlockCommand, address, 0);
	waitForAnswer(0xFE); // Start block token

	for (int i = 0; i < BLOCKSIZE; i++)
	{
		buff[i] = receiveByte();
	}

	//read, but ignore 16 bit CRC
	SPI_MasterTransmit(0xFF);
	SPI_MasterTransmit(0xFF);
}
