#include <stdint.h>
#include <stdlib.h>

#include <Port.h>
#include <OneWire.h>
#include <UART.h>
#include <StringBuilder.h>

constexpr Port ServoPort = Port::B;
constexpr uint8_t ServoPin = 0;
using ServoPins = Pins<ServoPort>;

constexpr int SERVO_MIN = 815;
constexpr int SERVO_MAX = 2000;

enum class DirectionIndex {
    Left = 1,
    Right = 2,
    Middle = 3
};

enum class Direction {
    Left = SERVO_MIN,
    Right = SERVO_MAX,
    Middle = SERVO_MIN + ((SERVO_MAX - SERVO_MIN) / 2)
};

Direction currentDirection = Direction::Middle;

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

int main() {
    ServoPins::setAllOutput();

    UART::setBaud(9600);
    UART::enableAsync();

    sei();

    while (true) {
        moveServo(currentDirection);
    }

}

StringBuilder<100, '\n'> strBuilder;

ISR(USART_RXC_vect) {
    const bool strComplete = strBuilder.add(UDR);

    if(strComplete) {
        const int newIndex = strBuilder.get().to_intger();

        Direction newDirection = currentDirection;

        switch (newIndex) {
            case static_cast<int>(DirectionIndex::Left):
                newDirection = Direction::Left;
                break;
            case static_cast<int>(DirectionIndex::Right):
                newDirection = Direction::Right;
                break;
            case static_cast<int>(DirectionIndex::Middle):
                newDirection = Direction::Middle;
                break;
            default:
                UART::writeString("This is not valid!\n");
        }

        currentDirection = newDirection;
    }
}
