#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <UART.h>
#include <stdio.h>

#define I2C_SCK 400000
#define SLA_R 0xA1
#define SLA_W 0xA0

#define  TW_START 0x8
#define  TW_SLAW_ACK 0x18
#define  TW_SLAR_ACK 0x40
#define  TW_WRITE_ACK 0x28
#define  TW_READ_ACK 0x50

unsigned char DATA[] = "Ottos Mops Hopst"; //{0x58, 0x00, 0x00, 0x00, 0x00, 0x01}; //CMD24, write block to address 0 0 0 0, (response R1 = 0x00)

char I2C_tx(uint8_t data, uint8_t successState) {
    TWDR = data; //CHIP_ADDR //Load SLA_W into TWDR Register.
    TWCR = (1 << TWINT) | (1 << TWEN); //Clear TWINT bit in TWCR to start transmission
    while (!(TWCR & (1 << TWINT))) //of address
        ;

    PORTB = ~TWSR;

    return ((TWSR & 0xF8) == successState); // == MT_SLA_ACK, SLA+W has been transmitted; ACK received

}

void I2C_Init() {
    TWSR = 0x00; //TWPS1 und TWPS0 = 0
    TWBR = static_cast<uint8_t>(F_CPU / I2C_SCK - 16); //I2C_SCK ist die gewünschte Frequenz
}

void I2C_Start() {
    //Send START condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    //Wait for TWINT Flag set. This indicates that the START condition has been transmitted
    while ((TWCR & (1 << TWINT)) == 0);

    PORTB = ~TWSR;

    //return (TWSR & 0xF8);
    auto erg = (TWSR & 0xF8);

    char str[100];
    sprintf(str, "Errorcode: %02X\r\n", erg);

    if (erg != TW_START){
        UART::writeString(str);
    }

}

void I2C_Stop() {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);

    PORTB = ~TWSR;
}

int main() {
    //I/0 Pins vom B-Register mit LEDs verbunden
    DDRB = 0xFF;
    PORTB = 0xFF;

    //USART Settings einstellen
    UART::enableSync();
    UART::setBaud(9600);

    //write data
    I2C_Init();
    I2C_Start();
    UART::writeString("I2C started\r\n");
    I2C_tx(SLA_W, TW_SLAW_ACK);


    for (int i = 0; DATA[i] != '\0'; i++) {
        I2C_tx(DATA[i], TW_WRITE_ACK);
    }

    UART::writeString("I2C tx ende\r\n");

    I2C_Stop();

    I2C_Start();

    UART::writeString("I2C started 2\r\n");

    //read data
    I2C_tx(SLA_R, TW_SLAR_ACK);
    I2C_tx(SLA_R, TW_SLAR_ACK);
    I2C_tx(SLA_R, TW_SLAR_ACK);


    return 0;
}

