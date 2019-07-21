#include <avr/interrupt.h>

#include <Pins.h>
#include <Timer.h>

#include "Song.h"

// Forward declarations

void createSong();
void flashLed(Note note);

// Port and Pin declarations

using Switches = Pins<Port::D>;
using Speaker = Pins<Port::C>;
using LEDs = Pins<Port::B>;

// Constants

constexpr Prescaler Timer0Prescaler = Prescaler::P1024;
constexpr Prescaler Timer1Prescaler = Prescaler::P1024;

constexpr uint16_t StartDelay = 100;
constexpr size_t PrescalerValue = 1024;
constexpr size_t PauseTime = 50;

constexpr size_t MaxSongLength = 40;

// Object declarations

bool pause = false;
int count = 0;
Song<MaxSongLength> song;

// Functions

int main() {
    Switches::setAllInput();
    Speaker::setAllOutput();
    LEDs::setAllOutput();

    createSong();

    Timer::Timer0::enableCompareMatch();
    Timer::Timer0::enableClearOnCompareMatch();
    Timer::Timer0::setPrescaler(Timer0Prescaler);

    Timer::Timer1::enableCompareMatch();
    Timer::Timer1::enableClearOnCompareMatch();
    Timer::Timer1::setPrescaler(Timer1Prescaler);

    Timer::Timer1::setCompareRegister(StartDelay);

    sei();

    while (true);
}

ISR (TIMER0_COMP_vect) {
    //Invert current value of the speaker to creating the oscillation
    Speaker::writeMaskInverted(Speaker::readAllPorts());
}

ISR (TIMER1_COMPA_vect) {
    if(pause) {
        Timer::Timer0::disableCompareMatch();
        Timer::Timer0::setCompareRegisterByTime(Timer0Prescaler, PauseTime);

        pause = false;

        return;
    }

    flashLed(song.notes[count]);

    if(song.notes[count] == Note::pause) {
        Timer::Timer0::disableCompareMatch();
    } else {
        Timer::Timer0::enableCompareMatch();
    }

    Timer::Timer0::setCompareRegisterByFrequency(Timer0Prescaler, (size_t) song.notes[count]);
    Timer::Timer1::setCompareRegisterByTime(Timer1Prescaler, (size_t) song.noteLengths[count]);

    count = ((count + 1) % song.size);
    pause = true;
}

void createSong() {
    song.addNote(Note::c, NoteLength::achtel);
    song.addNote(Note::d, NoteLength::achtel);
    song.addNote(Note::e, NoteLength::achtel);
    song.addNote(Note::f, NoteLength::achtel);
    song.addNote(Note::g, NoteLength::viertel);
    song.addNote(Note::g, NoteLength::viertel);
    song.addNote(Note::a, NoteLength::achtel);
    song.addNote(Note::a, NoteLength::achtel);
    song.addNote(Note::a, NoteLength::achtel);
    song.addNote(Note::a, NoteLength::achtel);
    song.addNote(Note::g, NoteLength::viertel);
    song.addNote(Note::pause, NoteLength::viertel);
    song.addNote(Note::a, NoteLength::achtel);
    song.addNote(Note::a, NoteLength::achtel);
    song.addNote(Note::a, NoteLength::achtel);
    song.addNote(Note::a, NoteLength::achtel);
    song.addNote(Note::g, NoteLength::viertel);
    song.addNote(Note::pause, NoteLength::viertel);
    song.addNote(Note::f, NoteLength::achtel);
    song.addNote(Note::f, NoteLength::achtel);
    song.addNote(Note::f, NoteLength::achtel);
    song.addNote(Note::f, NoteLength::achtel);
    song.addNote(Note::e, NoteLength::viertel);
    song.addNote(Note::e, NoteLength::viertel);
    song.addNote(Note::g, NoteLength::achtel);
    song.addNote(Note::g, NoteLength::achtel);
    song.addNote(Note::g, NoteLength::achtel);
    song.addNote(Note::g, NoteLength::achtel);
    song.addNote(Note::c, NoteLength::viertel);

    song.addNote(Note::pause, NoteLength::ganz);
}

void flashLed(Note note) {
    uint8_t mask = 0;

    switch(note) {
        case Note::c:
            mask |= 1 << 7;
            break;
        case Note::d:
            mask |= 1 << 6;
            break;
        case Note::e:
            mask |= 1 << 5;
            break;
        case Note::f:
            mask |= 1 << 4;
            break;
        case Note::g:
            mask |= 1 << 3;
            break;
        case Note::a:
            mask |= 1 << 2;
            break;
        case Note::h:
            mask |= 1 << 1;
            break;
        case Note::c1:
            mask |= 1 << 0;
            break;
    }

    LEDs::writeMaskInverted(mask);
}


