//
// Created by konrad on 01.04.19.
//

#ifndef AVR_CLOCK_H
#define AVR_CLOCK_H

#include <stdint.h>
#include <stddef.h>

template<typename T>
constexpr T calcCompTime(uint16_t prescaler, size_t millis) {
    return ((F_CPU / (double) prescaler) * (millis / 1000.0)) - 1;
}

template<typename T>
constexpr T calcCompFreq(uint16_t prescaler, size_t hz) {
    return ((F_CPU / (double) prescaler)  / hz) - 1;
}

#endif //AVR_CLOCK_H
