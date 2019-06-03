//
// Created by julian on 03.06.19.
//

#ifndef AVR_TEMPSENSOR_H
#define AVR_TEMPSENSOR_H

#include <stdio.h>

#include "OneWire.h"

template<Port OneWirePort, uint8_t OneWirePin>
class TempSensor {

    using MyOneWire = OneWire<OneWirePort, OneWirePin>;

public:
    static float readTemp() {
        constexpr uint8_t CONVERT_TEMP_CMD = 0x44;
        constexpr uint8_t READ_SCRATCHPAD_CMD = 0xbe;

        selectDevice();
        MyOneWire::writeByte(CONVERT_TEMP_CMD);

        selectDevice();
        MyOneWire::writeByte(READ_SCRATCHPAD_CMD);
        const auto scratchpad = MyOneWire::template readByteArray<9>();

        float erg = scratchpad[0] >> 1;

        if (isBitSet(scratchpad[0], 8)) {
            erg += 0.5;
        }

        if (scratchpad[1] != 0) {
            erg *= -1;
        }

        return erg;
    }

private:
    static void selectDevice() {
        constexpr uint8_t READ_ROM_CMD = 0x33;
        constexpr uint8_t MATCH_ROM_CMD = 0x55;

        MyOneWire::reset();
        MyOneWire::writeByte(READ_ROM_CMD);
        const auto address = MyOneWire::template readByteArray<8>();

        MyOneWire::reset();
        MyOneWire::writeByte(MATCH_ROM_CMD);
        MyOneWire::writeByteArray(address);
    }

    static bool isBitSet(uint8_t byte, uint8_t i) {
        return static_cast<bool>((byte & (1 << i)) >> i);
    }

};

#endif //AVR_TEMPSENSOR_H
