#include <avr/interrupt.h>

#include <Pins.h>
#include <Timer.h>

#include "Note.h"

// Forward declarations

void playSound();
void playNote(Note note);

// Constants

constexpr Prescaler Timer0Prescaler = Prescaler::P1024;

// Port and Pin declarations

using Switches = Pins<Port::A>;
using Speaker = Pins<Port::C>;
using LEDs = Pins<Port::B>;

// Functions

int main() {
    Switches::setAllInput();
    Speaker::setAllOutput();
    LEDs::setAllOutput();

    Timer::Timer0::enableClearOnCompareMatch();
    Timer::Timer0::setPrescaler(Timer0Prescaler);

    sei();

    while (true) {
        LEDs::writeMask(Switches::readAllPins());
        playSound();
    }

}

ISR (TIMER0_COMP_vect) {
    //Invert current value of the speaker to creating the oscillation
    Speaker::writeMaskInverted(Speaker::readAllPorts());
}

void playSound() {
    switch (Switches::readAllPins()) {
        case 0xff - 128:
            playNote(Note::c);
            break;
        case 0xff - 64:
            playNote(Note::d);
            break;
        case 0xff - 32:
            playNote(Note::e);
            break;
        case 0xff - 16:
            playNote(Note::f);
            break;
        case 0xff - 8:
            playNote(Note::g);
            break;
        case 0xff - 4:
            playNote(Note::a);
            break;
        case 0xff - 2:
            playNote(Note::h);
            break;
        case 0xff - 1:
            playNote(Note::c1);
            break;
        default:
            Timer::Timer0::disableCompareMatch();
    }
}

void playNote(Note note) {
    Timer::Timer0::setCompareRegisterByFrequency(Timer0Prescaler, (size_t) note);
    Timer::Timer0::enableCompareMatch();
}
