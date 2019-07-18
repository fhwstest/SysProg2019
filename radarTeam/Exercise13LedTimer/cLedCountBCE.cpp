#include <stddef.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include <Pins.h>
#include <Clock.h>

using LEDs = Pins<Port::B>;

uint8_t count = 0;

int main() {
    LEDs::setAllOutput();

    // Disable all LEDs (LEDS are active low)
    LEDs::writeAllHigh();

    // Enable Timer1 compare interrupt
    TIMSK |= (1<<OCIE1A);

    // Set compare register
    OCR1A = calcCompTime<uint16_t>(1024, 1000);

    // Clear Timer1 on compare match
    TCCR1B |= (1<<WGM12);

    // Start Timer1 with prescaler of 1024
    TCCR1B |= (1<<CS12)|(1<<CS10);

    sei();

    while(true);
}

ISR (TIMER1_COMPA_vect) {
    ++count;

    uint8_t left = count / 10;
    uint8_t right = count % 10;

    uint8_t erg = left << 4 | right;

    LEDs::writeMaskInverted(erg);
}
