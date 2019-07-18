#include <avr/interrupt.h>

#include <Pins.h>
#include <Clock.h>

#include "Note.h"

// Forward declarations

void playSound();
void playNote(Note note);

// Port and Pin declarations

using Switches = Pins<Port::A>;
using Speaker = Pins<Port::C>;
using LEDs = Pins<Port::B>;

// Functions

int main() {
    Switches::setAllInput();
    Speaker::setAllOutput();
    LEDs::setAllOutput();

    // Clear Timer0 on compare match
    TCCR0 = (1<<WGM01);

    // Start timer0 with prescaler 1024
    TCCR0 |= (1 << CS02) | (1 << CS00);

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
            //Disable Timer0
            TIMSK &= ~(1 << OCIE0);
    }
}

void playNote(Note note) {
    //Set timer compare register to manipulate the frequency of the speaker output
    OCR0 = calcCompFreq<uint8_t>(1024, static_cast<size_t>(note));

    //Enable Timer0
    TIMSK |= (1 << OCIE0);
}
