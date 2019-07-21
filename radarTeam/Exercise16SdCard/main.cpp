#include <avr/io.h>
#include <util/delay.h>

#include <stdio.h>

#include <UART.h>
#include <Pins.h>
#include <Array.h>

// Forward declarations

void initSpi();
void initSdCard();
void setBlockLength();
void writeByte(uint8_t address, uint8_t value);

void reset();

void sendCommand(uint8_t command, uint32_t argument, uint8_t crc = 0);
void waitForAnswer(uint8_t expectedAnswer);

Array<uint8_t, 6> buildMessage(uint8_t command, uint32_t argument, uint8_t crc);
void spiTransmit(uint8_t cData);

uint32_t uint32EndianConversion(uint32_t num);

// Constants

constexpr int MOSI = 6;
constexpr int SCK = 7;
constexpr int MISO = 5;
constexpr int CD = 3;
constexpr int SS = 4;
constexpr int EN1 = 1;
constexpr int EN2 = 2;

constexpr uint8_t initCrc = 74;

// Port and Pin declarations

using LEDs = Pins<Port::C>;
using SdCard = Pins<Port::A>;

// Functions

int main() {
    UART::enableSync();
    UART::setBaud(9600);

    initSpi();
    initSdCard();
    setBlockLength();
    writeByte(1, 1);

    while (true);
}

void initSpi() {
   // Set MOSI SCK etc. output, all others input
    SdCard::setPinOutput(MOSI);
    SdCard::setPinOutput(SCK);
    SdCard::setPinOutput(EN1);
    SdCard::setPinOutput(EN2);
    SdCard::setPinOutput(SS);

    // Enable SPI, Master, set clock rate fck/16
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

void initSdCard() {
    reset();

    SdCard::writePin(SS, false);
    _delay_ms(10);
    SdCard::writePin(SS, true);

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

void reset() {
    // Wait for SD Card to be online
    _delay_ms(300);

    SdCard::writePin(EN1, true);
    SdCard::writePin(EN2, false);

    _delay_ms(100);

    SdCard::writePin(EN1, false);
    SdCard::writePin(EN2, true);
}

void writeByte(uint8_t address, uint8_t value) {
    constexpr uint8_t writeBlockCommand = 24;
    sendCommand(writeBlockCommand, address);

    constexpr uint8_t startBlock = 254;
    spiTransmit(startBlock);

    spiTransmit(value);

    for (int i = 0; i < 511; ++i) {
        spiTransmit(0);
    }
}

void spiTransmit(uint8_t cData) {
    // Start transmission
    SPDR = cData;
    
    // Wait for transmission complete
    while (!(SPSR & (1 << SPIF)));
}

Array<uint8_t, 6> buildMessage(uint8_t command, uint32_t argument, uint8_t crc) {
    Array<uint8_t, 6> array(0);

    uint8_t *commandAddress = array.data();
    *commandAddress |= (1 << 6); // Set second bit to 1
    *commandAddress |= command; // Set command

    uint32_t *argumentAddress = reinterpret_cast<uint32_t *>(array.data() + 1);
    *argumentAddress = uint32EndianConversion(argument);

    uint8_t *crcAddress = array.data() + 5;
    crc = crc << 1;
    crc |= 1;
    *crcAddress = crc;

    return array;
}

void setBlockLength() {
    constexpr uint8_t setBlockLengthCommand = 16;
    sendCommand(setBlockLengthCommand, 512);
}

void sendCommand(uint8_t command, uint32_t argument, uint8_t crc) {
    auto array = buildMessage(command, argument, crc);

    for (uint8_t i : array) {
        spiTransmit(i);
    }
}

void waitForAnswer(uint8_t expectedAnswer) {
    while(SPDR != expectedAnswer) {
        spiTransmit(0xff);
    }
}

uint32_t uint32EndianConversion(uint32_t num) {
    return ((num >> 24) & 0xff) | // move byte 3 to byte 0
           ((num << 8) & 0xff0000) | // move byte 1 to byte 2
           ((num >> 8) & 0xff00) | // move byte 2 to byte 1
           ((num << 24) & 0xff000000);
}
