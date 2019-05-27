#include <Pins.h>

#include <OneWire.h>
#include <UART.h>
#include <stdio.h>

using TemSensor = Pins<Port::C>;
using LEDs = Pins<Port::B>;

constexpr uint8_t OW_PIN = 0;

using MyOneWire = OneWire<TemSensor, OW_PIN>;

void reset() {
    const auto isPresent = MyOneWire::detectPresence();
    LEDs::writePin(0, !isPresent);
}

void selectDevice() {
    constexpr int addressSize = 8;
    uint8_t address[addressSize];

    reset();
    MyOneWire::writeByte(0x33);
    MyOneWire::readNBytes(address, addressSize);

    reset();
    MyOneWire::writeByte(0x55);
    MyOneWire::writeNBytes(address, addressSize);
}

bool isBitSet(uint8_t byte, uint8_t i) {
    return static_cast<bool>((byte & (1 << i)) >> i);
}

float readTemp() {
    constexpr int scratchpadSize = 9;
    uint8_t scratchpad[scratchpadSize];

    selectDevice();
    MyOneWire::writeByte(0x44);

    selectDevice();
    MyOneWire::writeByte(0xbe);
    MyOneWire::readNBytes(scratchpad, scratchpadSize);

    float erg = scratchpad[0] >> 1;

    if (isBitSet(scratchpad[0], 8)) {
        erg += 0.5;
    }

    if(scratchpad[1] != 0) {
        erg *= -1;
    }

    return erg;
}

int main() {
    LEDs::setAllOutput();
    LEDs::writeAllHigh();

    UART::enableSync();
    UART::setBaud(9600);

    while (true) {
        float temp = readTemp();

        char str[19];
        sprintf(str, "Temp: %d \r\n", static_cast<int>(temp));
        UART::writeString(str);
    }
}