/********************************************************************
* Sends 512 Byte to a SD Card and reads back the block from address 0
* SD card connected to PORTB (SPI I/O)
* SPI host, so send card to SPI mode (Hardware SPI Mode)
* See: https://www.sdcard.org/downloads/pls/pdf/part1_410.pdf
* See: http://elm-chan.org/docs/mmc/mmc_e.html 
* See: http://www.uni-koblenz.de/~physik/informatik/ECC/sd.pdf 
********************************************************************/
//#define F_CPU 8000000UL 

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "hterm_short.h"  //for use of printf(), alternatives may be used

//Definition of bits, refer to "C-Control Pro SD-Card Interface"
#define WP   PB0  //low = write enable
#define CD   PB1
#define EN2  PB2
#define EN1  PB3
#define SS   PB4  //default PIN ATmega 
#define MOSI PB5  //default PIN ATmega
#define MISO PB6  //default PIN ATmega
#define SCK  PB7  //default PIN ATmega
//#define LED ??  anywhere

//Definition of global variables/arrays 
unsigned char buffer[512];   //buffer for data exchange

//Command tokens have the following coding scheme: see SD card Spec. page 8
//Start bit:       always'0', 
//Transmitter bit: '1'= host command
//Command content: command and address information or parameter, protected by 7 bit CRC checksum
//End bit:         always '1'
//
//Response tokens have one of four coding schemes, depending on their content 
//Respose Tokens (R1, ...) see Spec page 169: 7.3.2 Responses (4.9 page 80)
//
//CRC: see Spec. pages 62,63
//In the CMD line MSB is transmitted first, LSB is the last.

unsigned char CMD0[] =   {0x40, 0x00, 0x00, 0x00, 0x00, 0x95}; //CMD0 = Put card in SPI mode (response must be 0x01)
                                                               //0x95 = "1001010" 1, CRC Sum + end bit
unsigned char CMD55[] =  {0x77, 0x00, 0x00, 0x00, 0x00, 0x01}; //CMD55, indicates to card that next command is ACMD 
unsigned char ACMD41[] = {0x69, 0x00, 0x00, 0x00, 0x00, 0x01}; //ACMD41, activates card's initialization process (response must be 0x00)
unsigned char CMD17[] =  {0x51, 0x00, 0x00, 0x00, 0x00, 0x01}; //CMD17, read block from address 0 0 0 0, (response R1 = 0x00) 
unsigned char CMD24[] =  {0x58, 0x00, 0x00, 0x00, 0x00, 0x01}; //CMD24, write block to address 0 0 0 0, (response R1 = 0x00)  
//unsigned char CMD1[] =  {0x41, 0x00, 0x00, 0x00, 0x00, 0x95}; //CMD1 (response must be 0x00)
//unsigned char CMD8[] =  {0x48, 0x00, 0x00, 0x01, 0xAA, 0x87}; //CMD8 Check for voltage...
//unsigned char CMD16[] = {0x50, 0x00, 0x00, 0x02, 0x00, 0x01}; //CMD16, set block size to 512 Bytes, response R1 = 0x00, default!!
//unsigned char CMD58[] = {0x7A, 0x00, 0x00, 0x00, 0x00, 0xFD}; //CMD58 match Vdd or not...

//Function Prototypes
void spi_init(void);
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
 unsigned int i;      //counter
 //DDRC = 0xFF;       //LEDs for debug purposes on Port C (output)
 stdout = &mystdout;  //setup stdio stream
 usart_init();        //UART to terminal on PC
    
 if ((PINB & (1 << CD)) != 0x00) {
    	printf("No card inserted\n");
	return 0;
	}

 if ((PINB & (1 << WP)) != 0x00) {
	printf("Card write protected");
	return 0;
	} 
 
 spi_init();
 
 if (sd_init() != 0) {       //if initialization is NOT OK
 	return 0;
 }

  for (i = 512; i > 0; i--)	 //filling buffer for writing (modulo 8 bit)
     buffer[i] = i;

  write_block(buffer);           //writing data to SD card;
 
  for (i = 0; i < 512; i++)      //cleaning buffer
    buffer[i] = 0;
 
  if (read_block(buffer))  {      //reading data from SD card and print out
     for (i = 0; i < 512; i++)  {
	   printf("%3d ", buffer[i]);
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
 // PORTB = (1 << EN2) | (1 << SS) | (1 << WP) | (1 << MOSI);

 // Enable SPI, Master, set clock rate fosc/64, Check SPI Clock Rate!!
 //After power-on or CMD0 [...] with 400KHz clock frequency.
 SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR1);  // |(1<<SPR0);
 printf("SPI Initialisation completed\n"); 
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
* send command to SD card
********************************************************************/
unsigned char sd_cmd(unsigned char *CMD, char *cmd_nr) { 
 unsigned char response = 0;  //operation status
 int count =0;
 spi_tx(CMD[0]);	//command index
 spi_tx(CMD[1]);	//4 bytes argument
 spi_tx(CMD[2]);
 spi_tx(CMD[3]);
 spi_tx(CMD[4]);
 spi_tx(CMD[5]);	//CRC

 if (strcmp(cmd_nr,"CMD0") == 0) {
  //count=0;
  do {                 //receive response from card 	
     spi_tx(0xFF);     //Shift response from Slave to Master 
     response = spi_rx();   
     count++;
	 printf("SD command %s response: 0x%02x, %d\n", cmd_nr, response, count); 
     } 
  while ((response != 0x01 ) && (count < 0x0f)); //as long as R1 not 1 AND not TIMEOUT
  //Slave pulls up MISO, when "nothing" is transmitted, so 0xff is recieved.
 }

 else {                //cmd_nr is CMD55 or ACDM41
  //count=0;
  do {                 //receive response from card 	
      spi_tx(0xFF);     //Shift response from Slave to Master OR just resend CMDxx
      response = spi_rx();   
      count++;
	  printf("SD command %s response: 0x%02x, %d\n", cmd_nr, response, count); 
      } 
  while ((response != 0x00 ) && (count < 0x03)); //as long as R1 not 0 AND not TIMEOUT
  //Slave pulls up MISO, when "nothing" is transmitted, so 0xff is recieved.
  }
 return response;
}

/********************************************************************
* SD Card initialization procedure
********************************************************************/
unsigned int sd_init(void) {
  unsigned char response = 0; //operation status
  unsigned int i;
//  unsigned int count;
  
  PORTB |= (1 << EN1);       //Power init sequence (see "C-Control Pro SD-Card Interface")
  PORTB &= ~(1 << EN2);
  _delay_ms(75);
  PORTB |= (1 << EN2);
  PORTB &= ~(1 << EN1);

  for (i = 0; i < 10; i ++) //"supply at least 74 SD clocks to the SD card with keeping CMD line to high"
      spi_tx(0xFF);         //see Spec: 6.4 Power Scheme (part1_410.pdf)
							
  PORTB &= ~(1 << SS);      //enabling ss, "In case of SPI mode, CS shall be held to high during 74 clock cycles" 
                            //PORTB inital value = 0
  //The interface (SPI mode or SD mode) is selected during the first reset command after power up (CMD0)
  do {
   response = sd_cmd(CMD0, "CMD0");    //CMD0 (response must be 0x01)
  }
  while (response != 0x01);
         
  do {
    sd_cmd(CMD55, "CMD55"); 
    response = sd_cmd(ACMD41, "ACMD41"); //ACMD41 (response must be 0x00)
    //The "in idle state" bit in R1 response of ACMD41: "0" indicates completion of initialization (page 155)
  } 
  while (response != 0x00);

  printf("SD Initialisation completed\n"); //, response: 0x%02x\n", response); 
  return response;
}

/********************************************************************
* Write block subroutine
********************************************************************/
/* Start Block Tokens: For Single Block Read, Single Block Write and Multiple Block Read:
   - First byte: Start Block 0xFE
   - Bytes 2-513 (depends on the data block length): User data
   - Last two bytes: 16 bit CRC. 
   see Spec. pages 172, 157 */

unsigned char write_block(unsigned char* buff) { 
 unsigned char response = 0; //operation status
 unsigned char ret_val = 0;
 unsigned int count;
  
 response = sd_cmd(CMD24, "CMD24");   //CMD24 (WRITE_BLOCK) 
   
   if (response == 0x00) {            //Response must be 0x00
        spi_tx(0xFE);               //0xFE is StartBlock Token, 
        for (count=0; count<512; count++)  //data to be send
           spi_tx(buff[count]);

        //For block oriented write data transfer, the CRC check bits are added to each data block.
        spi_tx(0xFF);	   //sending CRC16 (not used)
        spi_tx(0xFF);      //sending 2 x 8 Bit CRC
        
        spi_tx(0xFF);      //get response
        response = spi_rx();
	printf("write_block() response = 0x%02x\n", response); 

    if ((response & 0x1F) == 0x05) { //Data Response Token: Bit 4 = 0 & '010' = data accepted & end bit always '1' 
         count=0;
         do  {      //waiting for finish of the busy state
             spi_tx(0xFF);
	     response = spi_rx();
	     printf("write_block() waiting = 0x%02x, %d\n", response, count); 
             count++;
         }
	 while ((response != 0xFF) && (count < 0xff));  //0xFF no more busy

         if (count < 0xff) 
            ret_val = 1;  //true = OK
        } 
       }   
return ret_val;
}

/********************************************************************
* Read block subroutine
********************************************************************/
unsigned char read_block(unsigned char* buff) { 
 unsigned char response = 0; //operation status
 unsigned int count;
 unsigned char ret_val = 0; //result of operation
 
  response = sd_cmd(CMD17, "CMD17");   //CMD17 (READ_SINGLE_BLOCK) 
  if (response == 0x00) {              //response must be 0x00
    count=0;
    do {                   //waiting for beginnig of the data block
       spi_tx(0xFF);       //First byte: Start Block Token = 0xFE
       response = spi_rx();
       printf("read_block() = 0x%02x, %d\n", response, count); 
       count++;
     } 
     while ((response != 0xFE) && (count < 0xff)); //try 0xff times maximum

   if (count < 0xff) { 
      for (count=0; count<512; count++) {    //reading block of 512 bytes 
	     spi_tx(0xFF);                   //shift data from slave to master data register
	     buff[count] = spi_rx();
       }
	
    spi_tx(0xFF); //A CRC is appended to the end of each block ensuring data transfer integrity  
    spi_rx();	  //reading 16-bit CRC, but ignored
    spi_tx(0xFF);
    spi_rx();     //reading 16-bit CRC, but ignored 
    ret_val = 1;
   }
  }
 return ret_val;
}
