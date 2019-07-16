#ifndef AVR_ONEWIRE_H
#define AVR_ONEWIRE_H

#include <util/delay.h>
#include <avr/interrupt.h>
#include <stddef.h>

#include "Array.h"
#include "Pins.h"

template<Port OneWirePort, uint8_t OW_PIN>
class OneWire {

    using OneWirePins = Pins<OneWirePort>;

public:
    static void writeBit0() {
        const auto sregSave = SREG;
        cli();

        OneWirePins::setAllOutput();
        OneWirePins::writePin(OW_PIN, false);
        _delay_us(60);
        OneWirePins::writePin(OW_PIN, true);
        _delay_us(10);

        SREG = sregSave;
    }

    static void writeBit1() {
        const auto sregSave = SREG;
        cli();

        OneWirePins::setAllOutput();
        OneWirePins::writePin(OW_PIN, false);
        _delay_us(6);
        OneWirePins::writePin(OW_PIN, true);
        _delay_us(64);

        SREG = sregSave;
    }

    static bool readBit() {
        const auto sregSave = SREG;
        cli();

        OneWirePins::setAllOutput();
        OneWirePins::writePin(OW_PIN, false);
        _delay_us(6);
        OneWirePins::setAllInput();
        _delay_us(9);
        const auto erg = OneWirePins::readSinglePin(OW_PIN);
        _delay_us(55);

        SREG = sregSave;

        return erg;
    }

    static bool reset() {
        const auto sregSave = SREG;
        cli();

        OneWirePins::setAllOutput();
        OneWirePins::writePin(OW_PIN, false);
        _delay_us(480);
        OneWirePins::setAllInput();
        _delay_us(70);
        const auto erg = OneWirePins::readSinglePin(OW_PIN);
        _delay_us(410);

        SREG = sregSave;

        return !erg;
    }

    static void writeByte(uint8_t data) {
        for (int i = 0; i < 8; i++) {
            const bool bit = static_cast<bool>((data & (1 << i)) >> i);

            if (bit) {
                writeBit1();
            } else {
                writeBit0();
            }
        }
    }

    static void writeNBytes(uint8_t* data, int dataSize) {
        for(int i = 0; i < dataSize; i++) {
            writeByte(data[i]);
        }
    }

    template <size_t N>
    static void writeByteArray(const Array<uint8_t, N>& array) {
        for(auto byte : array) {
            writeByte(byte);
        }
    }

    static uint8_t readByte() {
        uint8_t data{};

        for (int i = 0; i < 8; i++) {
            const bool bit = readBit();

            if (bit) {
                data = (static_cast<uint8_t>(data | (1 << i)));
            } else {
                data = (static_cast<uint8_t>(data & ~(1 << i)));
            }
        }

        return data;
    }

    static void readNBytes(uint8_t* data, int dataSize) {
        for(int i = 0; i < dataSize; i++) {
            data[i] = readByte();
        }
    }

    template <size_t N>
    static Array<uint8_t, N> readByteArray() {
        Array<uint8_t, N> array;

        for(auto& byte : array) {
            byte = readByte();
        }

        return array;
    }

};

#endif //AVR_ONEWIRE_H
