/*
* Ueb5A15e.c
*
* Created: 01.04.2019 23:05:26
* Author : Dietrich
* 
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "hterm_short.h"  //for use of printf(), alternatices may be used

#define I2C_SCK 400000 //EPROM mit 400 Kbit/s betrieben

#define SLA_R 0xA1     //Slaveadr. für Read:  1010 Postfix + A2 Pin 0 + 1 (Read)
#define SLA_W 0xA0     //Slaveadr. für Write: 1010 Postfix + A2 Pin 0 + 0 (Write)

#define MEM_ADR 0x01   //Speicheradresse im EEPROM

#define  TW_START_ACK 0x8    //OK Ack der Startbed.
#define  TW_SLAW_ACK 0x18    //OK Ack SLA_W
#define  TW_SLAR_ACK 0x40    //OK Ack SLA_R
#define  TW_WRITE_ACK 0x28   //OK Ack Write
#define  TW_READ_ACK 0x50    //OK Ack Read

void i2c_init(void);
void i2c_start(void);
char i2c_tx(char, char);
void i2c_stop(void);
unsigned char i2c_restart(void);
unsigned char i2c_data_rx(void);

int main() {
	
	//I/0 Pins vom B-Register mit LEDs verbunden
	DDRB = 0xFF;
	PORTB = 0xFF;
	
	//USART Settings einstellen
	stdout = &mystdout; //setup stdio stream
	usart_init();
	printf("usart started\n");

	//Init i2c
	i2c_init();

	//Write
	i2c_start();	
	i2c_tx(SLA_W, TW_SLAW_ACK); //Sendewunsch.
	i2c_tx(MEM_ADR, TW_WRITE_ACK); //EEPROM Speicheradresse
	i2c_tx(0xFA, TW_WRITE_ACK); //Daten schreiben
	i2c_stop();
	
	_delay_ms(200);

	//Read
	i2c_start();
	i2c_tx(SLA_W, TW_SLAW_ACK);    //Sendewunsch.
	i2c_tx(MEM_ADR, TW_WRITE_ACK); //EEPROM Speicheradresse einstellen, aus der wir lesen möchten
	i2c_restart();                 //Kein Stop, damit uns keiner den Mastermode klaut
	i2c_tx(SLA_R, TW_SLAR_ACK);    //Lesewunsch.
	i2c_data_rx();                 //Lesen	
	i2c_stop();	

	return 0;
}


//Init ----------------------------------------------------------------
void i2c_init()
{
	TWSR = 0x00; //TWPS1 und TWPS0 = 0  => (Prescaler = 1)
	TWBR = F_CPU / I2C_SCK - 16; //I2C_SCK ist die gewünschte Frequenz. Eigentlich noch * Prescaler.
	//Der ist bei uns aber 1.

	printf("i2c initialized.\n");

}

//Start condition-------------------------------------------------------
void i2c_start(void) {

	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN); //Send START condition
	while ((TWCR & (1<<TWINT)) == 0) //Wait for TWINT Flag set. This indicates that
	; //the START condition has been transmitted
	
	PORTB=~TWSR;

  	printf("i2c_start: 0x%02X \n", TWSR);
	
	if ((TWSR & 0xF8) != TW_START_ACK)
	printf("start error\r\n");
}

//Restart condition-------------------------------------------------------
unsigned char i2c_restart(void) {

	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
	;
	
	printf("i2c_restart: 0x%02X \n", TWSR);
	//_delay_ms(1000);

	PORTB=~TWSR;

	return ((TWSR & 0xF8) == 0x10);
}

//Stop i2c communication------------------------------------------------------------
void i2c_stop()
{
	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN); //Stop Bedingung senden. TWSTO
	PORTB=~TWSR;
	//Nicht auf TWINT prüfen. Wird beim TWSTO nicht vom MC gesetzt!
}

//Data send------------------------------------------------------------
char i2c_tx(char data, char successState)
{
	TWDR = data; //CHIP_ADDR //Load SLA_W into TWDR Register.
	TWCR = (1<<TWINT) | (1<<TWEN); //Clear TWINT bit in TWCR to start transmission
	while (!(TWCR & (1<<TWINT))) //of address
	;
		
	printf("i2c_tx: 0x%02X \n", TWSR);
	
	PORTB=~TWSR;

	return ((TWSR & 0xF8) == successState); // == MT_SLA_ACK, SLA+W has been transmitted; ACK received

}

//Data receive------------------------------------------------------------
unsigned char i2c_data_rx(void) {
	
	unsigned char data;
	
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)))
	;
	
	data = TWDR;
	
	printf("i2c_data_rx: 0x%02X \n", data);
	return data;
}


