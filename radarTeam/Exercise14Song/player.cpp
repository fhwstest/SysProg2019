#include <avr/interrupt.h>

#include <Clock.h>
#include <Pins.h>

#include "Song.h"

// Forward declarations

void createSong();
void flashLed(Note note);

// Port and Pin declarations

using Switches = Pins<Port::D>;
using Speaker = Pins<Port::C>;
using LEDs = Pins<Port::B>;

// Constants

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

    // Enable Timer1
    TIMSK |= (1 << OCIE1A);

    // Enable Timer0
    TIMSK |= (1 << OCIE0);

    // Set compare register for Timer1
    OCR1A = StartDelay;

    // Clear Timer0 on compare match
    TCCR0 = (1<<WGM01);

    // Clear Timer1 on compare match
    TCCR1B = (1<<WGM12);

    // Start Timer1 with prescaler of 1024
    TCCR1B |= (1 << CS12) | (1 << CS10);

    // Start Timer0 with prescaler of 1024
    TCCR0 |= (1 << CS02) | (1 << CS00);

    sei();

    while (true);
}

ISR (TIMER0_COMP_vect) {
    //Invert current value of the speaker to creating the oscillation
    Speaker::writeMaskInverted(Speaker::readAllPorts());
}

ISR (TIMER1_COMPA_vect) {
    if(pause) {
        // Disable Timer0
        TIMSK &= ~(1 << OCIE0);
        // Pause for given PauseTime
        OCR1A = calcCompTime<uint16_t>(PrescalerValue, PauseTime);

        pause = false;

        return;
    }

    flashLed(song.notes[count]);

    if(song.notes[count] == Note::pause) {
        //Disable Timer0
        TIMSK &= ~(1 << OCIE0);
    } else {
        //Enable Timer0
        TIMSK |= (1 << OCIE0);
    }

    OCR0 =  calcCompFreq<uint8_t>(PrescalerValue, static_cast<size_t>(song.notes[count]));
    OCR1A = calcCompTime<uint16_t>(PrescalerValue, static_cast<size_t>(song.noteLengths[count]));

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


