/*
* main.c
* SD-Card
* Created: 17.04.2019 23:34:31
* Author : Dietrich
*
* SPI liegt auf PORTB
*/

#include <avr/io.h>
#include <util/delay.h>
#include "hterm_short.h"  //for use of printf(), alternatices may be used

//Conrad - C-Control Pro SD-Card Interface - PIN.
// Freie PIN Auswahl
#define EN1  PB1
#define EN2  PB2
#define LED  PB3

//ATmega SPI Pin-Belegung
#define SS   PB4  //default PIN ATmega
#define MOSI PB5  //default PIN ATmega
#define MISO PB6  //default PIN ATmega
#define SCK  PB7  //default PIN ATmega

//Definition of global variables
unsigned int i;              //counter
unsigned char status;        //operation status
unsigned char buffer[512];   //buffer for data exchange

//Command tokens have the following coding scheme: see Spec. page 8
//Start bit:       always'0',
//Transmitter bit: '1'= host command
//Command content: command and address information or parameter, protected by 7 bit CRC checksum
//End bit:         always '1'
//Response tokens have one of four coding schemes, depending on their content. Spec. Part1 Page. 167
//In the CMD line MSB is transmitted first, LSB is the last.
//CRC: see Spec. pages 62,63

//*** Resets the SD Memory Card ***
unsigned char CMD0[] =   {0x40, 0x00, 0x00, 0x00, 0x00, 0x95}; //CMD0 = Put card in SPI mode (response must be 0x01)
//0x40 = "01000000"
//0x95 = "10010100" 1, CRC Sum + end bit
//Respose Tokens (R1, ...) see Spec page 169: 7.3.2 Responses

//*** Defines to the card that the next command is an application specific command rather than a standard command. ***
unsigned char CMD55[] =  {0x77, 0x00, 0x00, 0x00, 0x00, 0x01}; //CMD55, das "Prefix" für ein ACMD
//0x55 = 0011 0111 + StartBit&TransmitterBit = 0x77 = 0111 0111

// ***
//unsigned char CMD16[] = {0x50, 0x00, 0x00, 0x02, 0x00, 0x01}; //CMD16, set block size to 512 Bytes, response R1 = 0x00, default!!

//*** READ_SINGLE_BLOCK - Reads a block of the size selected by SET_BLOCKLEN command (note 3) ***
unsigned char CMD17[] =  {0x51, 0x00, 0x00, 0x00, 0x00, 0x01}; //CMD17, read block from address 0 0 0 0, (response R1 = 0x00)

//*** WRITE_BLOCK - Writes a block of the size selected by the SET_BLOCKLEN command. (note 4)
unsigned char CMD24[] =  {0x58, 0x00, 0x00, 0x00, 0x00, 0x01}; //CMD24, write block to address 0 0 0 0, (response R1 = 0x00)

/*** Sends host capacity support information and activates the card's initialization process. Reserved bits shall be set to '0'
SPI_Part1 - Page 149
ACMD41 is a synchronization command used to negotiate the operation voltage range and to poll the
cards until they are out of their power-up sequence. In case the host system connects multiple cards,
the host shall check that all cards satisfy the supplied voltage. Otherwise, the host should select one
of the cards and initialize.
*/
unsigned char ACMD41[] = {0x69, 0x00, 0x00, 0x00, 0x00, 0x01}; //ACMD41, activates card's initialization process (response must be 0x00)

//Function Prototypes
void spi_init(void);
void spi_slaveselect(void);
void spi_slaveunselect(void);
unsigned int sd_init(void);
void spi_tx(unsigned char);
unsigned char spi_rx(void);
unsigned char sd_cmd(unsigned char *, char *);
unsigned char write_block(unsigned char *);
unsigned char  read_block(unsigned char *);

/********************************************************************
* Main
********************************************************************/
int main(void) {
	
	DDRA = 0xFF; //LEDs on Port A (output)
	
	stdout = &mystdout; //setup stdio stream

	usart_init(); //UART to terminal on PC
	
	spi_init(); //SPI Controlregister und PORTB Pins setzen
	
	if (sd_init() != 0) {       //if initialization is NOT OK
		return 0;
	}

	for (i = 512; i > 0; i--)     //filling buffer for writing (modulo 8 bit)
	buffer[i] = i;

	PORTA = write_block(buffer);   //writing data to SD card;
	
	for (i = 0; i < 512; i++)      //cleaning buffer
	buffer[i] = 0;
	
	if (read_block(buffer))        //reading data from SD card
	{
		for (i = 0; i < 512; i++)
		{
			//PORTA = ~buffer[i];          //Show data using LEDs
			printf("%3d", buffer[i]);
			_delay_ms(20);         //to see LEDs blinking
		}
	}
	while (1);
}

/********************************************************************
* SPI initialization procedure
********************************************************************/
void spi_init(void) {
	
	// Set MOSI, SCK, SS output, MISO input, some preset values by HW-Reset are NOT set again to e.g. 0 or 1
	DDRB = (1 << EN2) | (1 << EN1) | (1 << SS) | (1 << MOSI) | (1 << SCK);
	
	// DDRB &= ~(1 << MISO); // not necessary, default = 0

	// Enable SPI, Master, set clock rate F_CPU/64
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR1); //|(1<<SPR1);
	
	printf("SPI Initialize completed\n");
}

/********************************************************************
* SD Card initialization procedure
********************************************************************/
unsigned int sd_init(void) {
	
	unsigned char status = 0;

	printf("SDINIT");

	//power up  250ms 6.4 Power Scheme (part1_410.pdf)
	_delay_ms(300);
	
	//Reset - C-Control PRO Reset.
	PORTB |= (1 << EN1);       //Power init sequence (see "C-Control Pro SD-Card Interface")
	PORTB &= ~(1 << EN2);
	_delay_ms(100);            //mind. 75ms Rest hier 100 ms
	PORTB |= (1 << EN2);
	PORTB &= ~(1 << EN1);
	
	//"supply at least 74 SD clocks to the SD card with keeping CMD line to high"
	// line to high. In case of SPI mode, CS shall be held to high during 74 clock cycles.
	//see Spec: 6.4 Power Scheme (part1_410.pdf)
	//SD Karte Kondensator aufladen
	spi_slaveselect();
	_delay_ms(10);
	spi_slaveunselect();
	
	//switch to SPI Mode
	//In case of SPI host, CMD0 shall be the first command to send the card to SPI mode.
	spi_slaveselect();
	printf("Send CMDO");

	//PORTB inital value = 0
	do
	{
		// 	Resets the SD Memory Card
		status = sd_cmd(CMD0, "CMD0");    //CMD0 (response must be 0x01) = > Idle-State
		
	} while (status != 0x01);
	
	do
	{
		//Defines to the card that the next command is an application specific command rather than a standard command.
		sd_cmd(CMD55, "CMD55");
		
		//Wenn Antwort = 0x00 dann ist SD Karte rdy, d.h. wenn Karte "geladen" ist
		status = sd_cmd(ACMD41, "ACMD41"); //ACMD41 (response must be 0x00)
	}
	while (status != 0x00);

	printf("SD Initialisation completed, status: 0x%02x\n", status);
	return status;
}

/********************************************************************
* send command to SD card
********************************************************************/
unsigned char sd_cmd(unsigned char *CMD, char *cmd_nr) {
	unsigned char res;
	unsigned int count;
	spi_tx(CMD[0]);    //command index
	spi_tx(CMD[1]);    //4 bytes argument
	spi_tx(CMD[2]);
	spi_tx(CMD[3]);
	spi_tx(CMD[4]);
	spi_tx(CMD[5]);    //CRC
	
	count=0;
	do                 //receive response from card
	{
		spi_tx(0xff);    //Shift response from Slave to Master
		res = spi_rx();
		printf("SD command %s response: 0x%02x\n", cmd_nr, res);
		count++;
	}
	while (((res & 0x80) != 0x00) && (count < 0xFFFF)); //as long as Bit 7 of (ResponseToken) R1 = 1 (and not 0) and TIMEOUT
		
	//Slave pulls MISO up to high, when "nothing" is transmitted, so 0xff is always recieved.
	// Deshalb warten wir auf Bit 7  == 0 in vorheriger Schleife

	printf("SD command %s response: 0x%02x\n", cmd_nr, res);
	return res;
}


/********************************************************************
*    Slave auf Empfang setzen
********************************************************************/
void spi_slaveselect()
{
	PORTB &= ~(1 << SS);	
}

/********************************************************************
*    Slave Deselektieren
********************************************************************/
void spi_slaveunselect()
{
	PORTB |= (1 << SS);
}

/********************************************************************
* transmit byte via SPI
********************************************************************/
void spi_tx(unsigned char data) {
	SPDR = data;
	while (!(SPSR & (1 << SPIF)))  //Wait for transmission to complete
	;
}

/********************************************************************
* receive byte via SPI
********************************************************************/
unsigned char spi_rx(void) {
	while (!(SPSR & (1 << SPIF)))  //Wait for transmission to complete
	;
	return SPDR;
}

/********************************************************************
* Write block subroutine
********************************************************************/
unsigned char write_block(unsigned char* buff) {
	
	unsigned char ret_val = 0;
	unsigned int count;
	
	//Writes a block of the size selected by the SET_BLOCKLEN command. (note 4)
	//Startadresse ist die 0000 Handbuch Part1 173.
	status = sd_cmd(CMD24, "CMD24");   //CMD24 (WRITE_BLOCK)
	
	if (status == 0x00)     //Response must be 0x00
	{
		spi_tx(0xFE);    //0xFE is StartBlock Token, see Spec. pages 172, 157
		for (count=0; count<512; count++)    //data to be send
			spi_tx(buff[count]);

		//For block oriented write data transfer, the CRC check bits are added to each data block.
		//einfach nur Daten abholen, das SD Karte Sendepuffer leeren kann
		spi_tx(0xFF);       //sending CRC16 (not used)
		spi_tx(0xFF);      //sending 2 x 8 Bit CRC
		
		spi_tx(0xFF);
		status = spi_rx();
		printf("write_block() = 0x%02x\n", status);

		if ((status & 0x15) == 0x05) //Data Response Token: Bit 4 = 0 & '010' = data accepted & end bit always '1'
		{
			count=0;
			do        //waiting for finish of the busy state
			{
				spi_tx(0xFF);
				status = spi_rx();
				printf("write_block() = 0x%02x, %d\n", status, count);
				count++;
			}
			while ((status != 0xFF) && (count < 0xffff));

			if (count < 0xffff)
			ret_val = 1;  //true = OK
		}
	}
	return ret_val;
}

/********************************************************************
* Read block subroutine
********************************************************************/
unsigned char read_block(unsigned char* buff) {
	unsigned int count;
	unsigned char ret_val = 0; //result of operation
	
	status = sd_cmd(CMD17, "CMD17");   //CMD17 (READ_SINGLE_BLOCK)
	if (status == 0x00)       //response must be 0x00
	{
		count=0;

		do                   //waiting for beginnig of the data block
		{                    //First byte: Start Block Token = 0xFE
			spi_tx(0xFF);
			status = spi_rx();
			PORTA = ~status;
			printf("read_block() = 0x%02x, %d\n", status, count);
			count++;
		}
		while ((status != 0xFE) && (count < 0xFFFF)); //try 0xFFFF times maximum

		if (count < 0xFFFF)
		{
			for (count=0; count<512; count++){    //reading block of 512 bytes
				spi_tx(0xFF);               //shift data from slave to master data register
				buff[count] = spi_rx();
			}
			
			spi_tx(0xFF); //A CRC is appended to the end of each block ensuring data transfer integrity
			spi_rx();      //reading 16-bit CRC, but ignored
			spi_tx(0xFF);
			spi_rx();     //reading 16-bit CRC, but ignored
			ret_val = 1;
		}
	}
	return ret_val;
}


