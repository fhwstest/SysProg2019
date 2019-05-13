#ifndef AVR_ONEWIRE_H
#define AVR_ONEWIRE_H

#include <util/delay.h>
#include <avr/interrupt.h>

template<typename TemSensor, uint8_t OW_PIN>
class OneWire {

public:
    static void writeBit0() {
        const auto sregSave = SREG;
        cli();

        TemSensor::setAllOutput();
        TemSensor::writePin(OW_PIN, false);
        _delay_us(60);
        TemSensor::writePin(OW_PIN, true);
        _delay_us(10);

        SREG = sregSave;
    }

    static void writeBit1() {
        const auto sregSave = SREG;
        cli();

        TemSensor::setAllOutput();
        TemSensor::writePin(OW_PIN, false);
        _delay_us(6);
        TemSensor::writePin(OW_PIN, true);
        _delay_us(64);

        SREG = sregSave;
    }

    static bool readBit() {
        const auto sregSave = SREG;
        cli();

        TemSensor::setAllOutput();
        TemSensor::writePin(OW_PIN, false);
        _delay_us(6);
        TemSensor::setAllInput();
        _delay_us(9);
        const auto erg = TemSensor::readSinglePin(OW_PIN);
        _delay_us(55);

        SREG = sregSave;

        return erg;
    }

    static bool detectPresence() {
        const auto sregSave = SREG;
        cli();

        TemSensor::setAllOutput();
        TemSensor::writePin(OW_PIN, false);
        _delay_us(480);
        TemSensor::setAllInput();
        _delay_us(70);
        const auto erg = TemSensor::readSinglePin(OW_PIN);
        _delay_us(410);

        SREG = sregSave;

        return erg;
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

};

#endif //AVR_ONEWIRE_H
