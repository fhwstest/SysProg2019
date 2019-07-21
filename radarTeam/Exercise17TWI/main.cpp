#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <UART.h>
#include <SString.h>

#include <stdio.h>

#define I2C_SCK 400000
#define SLA_R 0xA1
#define SLA_W 0xA0

#define  TW_START 0x8
#define  TW_SLAW_ACK 0x18
#define  TW_SLAR_ACK 0x40
#define  TW_WRITE_ACK 0x28

void init();
void start();
void stop();

void writeData(uint8_t data, uint8_t successState, const char *msg);
uint8_t readData();

void printErg(const char* msg, uint8_t code, uint8_t expectedCode);

int main() {
    constexpr int address = 0;
    constexpr int data = 0x0b;

    //USART Settings einstellen
    UART::enableSync();
    UART::setBaud(9600);

    init();

    start();

    //write data
    writeData(SLA_W, TW_SLAW_ACK, "Set in Write Mode");
    writeData(address, TW_WRITE_ACK, "Set Address");
    writeData(data, TW_WRITE_ACK, "Write Data");

    stop();

    _delay_ms(200);

    start();
    writeData(SLA_W, TW_SLAW_ACK, "Set in write Mode 2");
    writeData(address, TW_WRITE_ACK, "Set read Address");

    start();
    writeData(SLA_R, TW_SLAR_ACK, "Set in read Mode");
    readData();

    stop();
}

void init() {
    TWSR = 0x00; //TWPS1 und TWPS0 = 0
    TWBR = static_cast<uint8_t>(F_CPU / I2C_SCK - 16); //I2C_SCK ist die gewünschte Frequenz
}

void start() {
    //Send START condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    //Wait for TWINT Flag set. This indicates that the START condition has been transmitted
    while ((TWCR & (1 << TWINT)) == 0);

    uint8_t erg = static_cast<uint8_t>(TWSR & 0xF8);

    printErg("Start", erg, TW_START);
}

void stop() {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void writeData(uint8_t data, uint8_t successState, const char *msg) {
    TWDR = data; //CHIP_ADDR //Load SLA_W into TWDR Register.
    TWCR = (1 << TWINT) | (1 << TWEN); //Clear TWINT bit in TWCR to start transmission
    while (!(TWCR & (1 << TWINT)));

    uint8_t erg = static_cast<uint8_t>(TWSR & 0xF8);

    printErg(msg, erg, successState);
}

uint8_t readData() {
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));

    uint8_t data = TWDR;

    UART::printf("Code: %02X\r\n", data);

    return data;
}

void printErg(const char *msg, uint8_t code, uint8_t expectedCode) {
    if(code != expectedCode) {
        UART::printf("Error - %s: %02X", msg, code);
    }
}
