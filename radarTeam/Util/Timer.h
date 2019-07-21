//
// Created by JulZi on 18.07.2019.
//

#include <avr/interrupt.h>

#include <stddef.h>

#include "Prescaler.h"

#ifndef AVR_TIMER_H
#define AVR_TIMER_H

class Timer {
public:
    class Timer0 {
    public:
        static void enable() {
            TIMSK |= (1 << OCIE0);
        }

        static void disable() {
            TIMSK &= ~(1 << OCIE0);
        }

        static constexpr void setCompareRegister(uint8_t value) {
            OCR0 = value;
        }

        static constexpr void setCompareRegisterByFrequency(Prescaler prescaler, size_t value) {
            OCR0 = calcCompFreq<uint8_t>((uint16_t) prescaler, value);
        }

        static constexpr void setCompareRegisterByTime(Prescaler prescaler, size_t value) {
            OCR0 = calcCompTime<uint8_t>((uint16_t) prescaler, value);
        }

        static constexpr void setPrescaler(Prescaler prescaler) {
            if (prescaler == Prescaler::P1024) {
                TCCR0 |= (1 << CS02) | (1 << CS00);
            }
        }

        static void enableClearOnCompareMatch() {
            TCCR0 |= (1<<WGM01);
        }

        static void disableClearOnCompareMatch() {
            TCCR0 &= ~(1 << WGM01);
        }

    };


    class Timer1 {
    public:
        static void enable() {
            TIMSK |= (1 << OCIE1A);
        }

        static void disable() {
            TIMSK &= ~(1 << OCIE1A);
        }

        static constexpr void setCompareRegister(uint16_t value) {
            OCR1A = value;
        }

        static constexpr void setCompareRegisterByFrequency(Prescaler prescaler, size_t value) {
            OCR1A = calcCompFreq<uint16_t>((uint16_t) prescaler, value);
        }

        static constexpr void setCompareRegisterByTime(Prescaler prescaler, size_t value) {
            OCR1A = calcCompTime<uint16_t>((uint16_t) prescaler, value);
        }

        static constexpr void setPrescaler(Prescaler prescaler) {
            if (prescaler == Prescaler::P1024) {
                TCCR1B |= (1 << CS12) | (1 << CS10);
            }
        }

        static void enableClearOnCompareMatch() {
            TCCR1B |= (1<<WGM12);
        }

        static void disableClearOnCompareMatch() {
            TCCR1B &= ~(1 << WGM12);
        }
    };


private:

    template<typename T>
    static constexpr T calcCompTime(uint16_t prescaler, size_t millis) {
        return ((F_CPU / (double) prescaler) * (millis / 1000.0)) - 1;
    }

    template<typename T>
    static constexpr T calcCompFreq(uint16_t prescaler, size_t hz) {
        return ((F_CPU / (double) prescaler)  / hz) - 1;
    }

};

#endif //AVR_TIMER_H
