//
// Created by JulZi on 18.07.2019.
//

#include <avr/interrupt.h>

#include "Prescaler.h"

#ifndef AVR_TIMER_H
#define AVR_TIMER_H

class Timer {

public:

    // Timer 0

    static void enableTimer0() {
        TIMSK |= (1 << OCIE0);
    }

    static void setCompareRegisterForTimer0(uint8_t value) {
        OCR0 = value;
    }

    template <Prescaler prescaler>
    static void setPrescalerForTimer0() {
        constexpr if(prescaler == Prescaler::P1024) {
            TCCR0 |= (1 << CS02) | (1 << CS00);
        }
    }

    static void enableClearOnCompareMatchForTimer0() {
        TCCR0 |= (1<<WGM01);
    }

    static void disableClearOnCompareMatchForTimer0() {
        TCCR0 &= ~(1 << WGM01)
    }

    // Timer 1

    static void enableTimer1() {
        TIMSK |= (1 << OCIE1A);
    }

    static void setCompareRegisterForTimer1(uint16_t value) {
        OCR1A = value;
    }

    template <Prescaler prescaler>
    static void setPrescalerForTimer1() {
        constexpr if(prescaler == Prescaler::P1024) {
            TCCR1B |= (1 << CS12) | (1 << CS10);
        }
    }

    static void enableClearOnCompareMatchForTimer1() {
        TCCR1B |= (1<<WGM12);
    }

    static void disableClearOnCompareMatchForTimer1() {
        TCCR1B &= ~(1 << WGM12)
    }

};

#endif //AVR_TIMER_H
