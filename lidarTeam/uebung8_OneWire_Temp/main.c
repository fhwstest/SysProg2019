/*
 * uebung8_OneWire_Temp.c
 *
 * Created: 15.07.2019 14:18:08
 * Author : PC
 */

#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include "../uebung5_USART/USART.h"

#define THERMOMOUT = PORTB
#define THERMOMMODE = DDRB
#define THERMOMIN = PINB0

int main(void)
{
	USART_Init(9600);
	int Temperature = -1000;
	/* Replace with your application code */
	while (1)
	{
		static float newTemp = readTemp();
		if (newTemp != Temperature)
		{
			Temperature = newTemp;
			USART_Transmit(Temperature);
		}
	}
}

static void writeBit0()
{
	const char sregSave = SREG;
	cli();
	THERMOMMODE = 0x01;
	THERMOMOUT = 0x00;
	_delay_us(60);
	THERMOMOUT = 0x01;
	_delay_us(10);

	SREG = sregSave;
}

static void writeBit1()
{
	const char sregSave = SREG;
	cli();

	THERMOMMODE = 0x01;
	THERMOMOUT = 0x00;
	_delay_us(6);
	THERMOMOUT = 0x01;
	_delay_us(64);

	SREG = sregSave;
}

static bool readBit()
{
	const char sregSave = SREG;
	cli();

	THERMOMMODE = 0x01;
	THERMOMOUT = 0x00;
	_delay_us(6);
	THERMOMMODE = 0x00;
	_delay_us(9);
	const int erg = THERMOMIN;
	_delay_us(55);

	SREG = sregSave;
	return erg;
}

static bool reset()
{
	const char sregSave = SREG;
	cli();

	THERMOMMODE = 0x01;
	THERMOMOUT = 0x00;
	_delay_us(480);
	THERMOMMODE = 0x00;
	_delay_us(70);
	const int erg = THERMOMIN;
	_delay_us(410);

	SREG = sregSave;
	return !erg;
}

static void writeByte(uint8_t data)
{
	for (int i = 0; i < 8; i++)
	{
		const int bit = ((data & (1 << i)) >> i);

		if (bit)
		{
			writeBit1();
		}
		else
		{
			writeBit0();
		}
	}
}

static void writeNBytes(uint8_t *data, int dataSize)
{
	for (int i = 0; i < dataSize; i++)
	{
		writeByte(data[i]);
	}
}

static uint8_t readByte()
{
	uint8_t data{};

	for (int i = 0; i < 8; i++)
	{
		const bool bit = readBit();

		if (bit)
		{
			data |= (1 << i);
		}
		else
		{
			data &= ~(1 << i);
		}
	}

	return data;
}

static void readNBytes(uint8_t *data, int dataSize)
{
	for (int i = 0; i < dataSize; i++)
	{
		data[i] = readByte();
	}
}

static float readTemp()
{
	const uint8_t CONVERT_TEMP_CMD = 0x44;
	const uint8_t READ_SCRATCHPAD_CMD = 0xbe;

	selectDevice();
	writeByte(CONVERT_TEMP_CMD);

	selectDevice();
	writeByte(READ_SCRATCHPAD_CMD);
	char scratchpad[9];
	readNBytes(scratchpad, 9);


	// shift bit out to get actual temp
	float erg = scratchpad[0] >> 1;

	if (isBitSet(scratchpad[0], 8))
	{
		erg += 0.5;
	}

	if (scratchpad[1] != 0)
	{
		erg *= -1;
	}

	return erg;
}

static void selectDevice()
{
	const uint8_t READ_ROM_CMD = 0x33;
	const uint8_t MATCH_ROM_CMD = 0x55;

	reset();
	writeByte(READ_ROM_CMD);
	char address[8];
	readNBytes(address, 8));

	reset();
	writeByte(MATCH_ROM_CMD);
	writeNBytes(address, 8);
}

static bool isBitSet(uint8_t byte, uint8_t i)
{
	return static_cast<bool>((byte & (1 << i)) >> i);
}
