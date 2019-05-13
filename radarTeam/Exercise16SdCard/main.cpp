#include <avr/io.h>
#include <avr/delay.h>

#include <stdio.h>

#include <UART.h>
#include <Pins.h>

using LEDs = Pins<Port::C>;
using SdCard = Pins<Port::B>;

constexpr int MOSI = 5;
constexpr int SCK = 7;
constexpr int MISO = 6;
constexpr int CD = 3;
constexpr int SS = 4;
constexpr int EN1 = 1;
constexpr int EN2 = 2;

constexpr uint8_t initCrc = 94;

void initSpi();
void initSdCard();

void reset();

void waitForAnswer(uint8_t expectedAnswer);

void writeByte(uint8_t address, uint8_t value);
void SPI_MasterTransmit(uint8_t cData);

void buildMessage(uint8_t command, uint32_t argument, uint8_t crc, uint8_t *array);
void sendCommand(uint8_t command, uint32_t argument, uint8_t crc = 0);

void setBlockLength();

void printErg(const char *msg);

uint32_t uint32EndianConversion(uint32_t num);

int main() {
    UART::enableSync();
    UART::setBaud(9600);

    initSpi();

    initSdCard();
    printErg("Init");

    setBlockLength();
    printErg("Set Block");

    writeByte(1, 1);
    printErg("Write Byte");

    while (true);
}

void waitForAnswer(uint8_t expectedAnswer) {
    while(SPDR != expectedAnswer) {
        SPI_MasterTransmit(0xff);
    }
}

void printErg(const char *msg) {
    char str[3];
    sprintf(str, "%02X", SPDR);

    UART::writeString(msg);
    UART::writeString(" war ");
    UART::writeString(SPDR == 0 ? "erfolgreich" : "nicht erfolgreich");
    UART::writeString(" Code: ");
    UART::writeString(str);
    UART::writeString("\r\n");
}

void initSpi() {
/* Set MOSI and SCK output, all others input */
    DDRB = (1 << MOSI) | (1 << SCK) | (1 << EN1) | (1 << EN2) | (1 << SS);

/* Enable SPI, Master, set clock rate fck/16 */
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);

    SdCard::writePin(SS, false);
}

void SPI_MasterTransmit(uint8_t cData) {
/* Start transmission */
    SPDR = cData;
/* Wait for transmission complete */
    while (!(SPSR & (1 << SPIF)));
}

uint32_t uint32EndianConversion(uint32_t num) {
    return ((num >> 24) & 0xff) | // move byte 3 to byte 0
           ((num << 8) & 0xff0000) | // move byte 1 to byte 2
           ((num >> 8) & 0xff00) | // move byte 2 to byte 1
           ((num << 24) & 0xff000000);
}

void buildMessage(uint8_t command, uint32_t argument, uint8_t crc, uint8_t *array) {
    uint8_t *commandAddress = array;
    *commandAddress |= (1 << 6); // Set second bit to 1
    *commandAddress |= command; // Set command

    uint32_t *argumentAddress = reinterpret_cast<uint32_t *>(array + 1);
    *argumentAddress = uint32EndianConversion(argument);

    uint8_t *crcAddress = array + 5;
    crc = crc << 1;
    crc |= 1;
    *crcAddress = crc;
}

void sendCommand(uint8_t command, uint32_t argument, uint8_t crc) {
    uint8_t array[6] = {};
    buildMessage(command, argument, crc, array);

    for (int i = 0; i < 6; i++) {
        SPI_MasterTransmit(array[i]);
    }
}

void reset() {
    SdCard::writePin(EN1, false);
    SdCard::writePin(EN2, true);

    SdCard::writePin(EN1, true);
    SdCard::writePin(EN2, false);

    _delay_ms(60);

    SdCard::writePin(EN1, false);
    SdCard::writePin(EN2, true);
}

void initSdCard() {
    reset();

    SdCard::writePin(SS, true);

    // Wait 10 cycles
    for(int i = 0; i < 10; i++) {
        SPI_MasterTransmit(0xff);
    }

    SdCard::writePin(SS, false);

    constexpr uint8_t initializeCommand = 0;
    sendCommand(initializeCommand, 0, initCrc);
    waitForAnswer(0x1);

    constexpr uint8_t enableApplicationCommand = 55;
    sendCommand(enableApplicationCommand, 0);
    waitForAnswer(0);

    constexpr uint8_t initSequenceCommand = 41;
    sendCommand(initSequenceCommand, 0);
    waitForAnswer(0);
}

void writeByte(uint8_t address, uint8_t value) {
    constexpr uint8_t writeBlockCommand = 24;
    sendCommand(writeBlockCommand, address);

    constexpr uint8_t startBlock = 254;
    SPI_MasterTransmit(startBlock);

    SPI_MasterTransmit(value);

    for (int i = 0; i < 511; ++i) {
        SPI_MasterTransmit(0);
    }
}

void setBlockLength() {
    constexpr uint8_t setBlockLengthCommand = 16;
    sendCommand(setBlockLengthCommand, 512);
}
