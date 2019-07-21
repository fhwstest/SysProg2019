#include <stdint.h>
#include <stdlib.h>

#include <Port.h>
#include <OneWire.h>
#include <UART.h>
#include <StringBuilder.h>

#include "Direction.h"

// Forward declarations

void moveServo(Direction direction);

// Port and Pin declarations

constexpr Port ServoPort = Port::B;
constexpr uint8_t ServoPin = 0;
using ServoPins = Pins<ServoPort>;

// Object declarations

Direction currentDirection = Direction::Middle;

// Functions

int main() {
    ServoPins::setAllOutput();

    UART::setBaud(9600);
    UART::enableAsync();

    sei();

    while (true) {
        moveServo(currentDirection);
    }

}

ISR(USART_RXC_vect) {
     const int newIndex = UDR;

    switch (newIndex) {
        case static_cast<int>(DirectionIndex::Left):
            currentDirection = Direction::Left;
            break;
        case static_cast<int>(DirectionIndex::Right):
            currentDirection = Direction::Right;
            break;
        case static_cast<int>(DirectionIndex::Middle):
            currentDirection = Direction::Middle;
            break;
        default:
            UART::writeLineN("This is not valid!");
    }
}

void moveServo(Direction direction) {
    ServoPins::writePin(ServoPin, false);
    _delay_ms(20);
    ServoPins::writePin(ServoPin, true);

    switch (direction) {
        case Direction::Left:
            _delay_us(static_cast<int>(Direction::Left));
            break;
        case Direction::Right:
            _delay_us(static_cast<int>(Direction::Right));
            break;
        case Direction::Middle:
            _delay_us(static_cast<int>(Direction::Middle));
    }

    ServoPins::writePin(ServoPin, false);
}
