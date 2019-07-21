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
        static void enableCompareMatch() {
            TIMSK |= (1 << OCIE0);
        }

        static void disableCompareMatch() {
            TIMSK &= ~(1 << OCIE0);
        }

        static constexpr void setCompareRegister(uint8_t value) {
            OCR0 = value;
        }

        template<typename U>
        static constexpr void setCompareRegisterByFrequency(Prescaler prescaler, U hz) {
            OCR0 = calcCompFreq<uint8_t>((uint16_t) prescaler, hz);
        }

        template<typename U>
        static constexpr void setCompareRegisterByTime(Prescaler prescaler, U millis) {
            OCR0 = calcCompTime<uint8_t>((uint16_t) prescaler, millis);
        }

        static constexpr void setPrescaler(Prescaler prescaler) {
            if (prescaler == Prescaler::P1024) {
                TCCR0 |= (1 << CS02) | (1 << CS00);
            }
        }

        static void enableClearOnCompareMatch() {
            TCCR0 |= (1<<WGM01);
        }
    };


    class Timer1 {
    public:
        static void enableCompareMatch() {
            TIMSK |= (1 << OCIE1A);
        }

        static void enableFastPWM() {
            TCCR1A |= (1 << WGM11);
            TCCR1B |= (1<<WGM12)|(1<<WGM13);
        }

        static void disableCompareMatch() {
            TIMSK &= ~(1 << OCIE1A);
        }

        static constexpr void setCompareRegister(uint16_t value) {
            OCR1A = value;
        }

        static constexpr void setCompareRegisterICR(uint16_t value) {
            ICR1 = value;
        }

        template<typename U>
        static constexpr void setCompareRegisterByFrequency(Prescaler prescaler, U hz) {
            OCR1A = calcCompFreq<uint16_t>((uint16_t) prescaler, hz);
        }

        template<typename U>
        static constexpr void setCompareRegisterByTime(Prescaler prescaler, U millis) {
            OCR1A = calcCompTime<uint16_t>((uint16_t) prescaler, millis);
        }

        static constexpr void setPrescaler(Prescaler prescaler) {
            if (prescaler == Prescaler::P1024) {
                TCCR1B |= (1 << CS12) | (1 << CS10);
            }

            if (prescaler == Prescaler::P64) {
                TCCR1B |= (1<<CS10) | (1<<CS11);
            }
        }

        static void enableClearOnCompareMatch() {
            TCCR1B |= (1<<WGM12);
        }

        static void enableClearOnCompareMatchICR() {
            TCCR1A |= (1<<COM1A1);
        }
    };


private:

    template<typename T, typename U>
    static constexpr T calcCompTime(uint16_t prescaler, U millis) {
        return ((F_CPU / (double) prescaler) * (millis / 1000.0)) - 1;
    }

    template<typename T, typename U>
    static constexpr T calcCompFreq(uint16_t prescaler, U hz) {
        return ((F_CPU / (double) prescaler)  / hz) - 1;
    }

};

#endif //AVR_TIMER_H
