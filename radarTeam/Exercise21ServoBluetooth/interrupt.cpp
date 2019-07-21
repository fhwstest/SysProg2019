#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <UART.h>
#include <Pins.h>
#include <Timer.h>

#include "Direction.h"

// Forward declarations

void initPWM();
void moveServo();

// Constants

constexpr Prescaler Timer1Prescaler = Prescaler::P64;
constexpr int DutyCycleTimeValue = 2499;

// Port and Pin declarations

constexpr Port ServoPort = Port::D;
constexpr uint8_t ServoPin = 5;
using ServoPins = Pins<ServoPort>;

// Object declarations

DirectionIndex currentDirection = DirectionIndex::Left;

// Functions

int main() {
    sei();

    UART::enableAsync();
    UART::setBaud(9600);

    ServoPins::setPinOutput(ServoPin);

    initPWM();

    while(true) {
        moveServo();
    }
}

ISR(USART_RXC_vect){
    currentDirection = (DirectionIndex) UDR;
}

void initPWM() {
    Timer::Timer1::setCompareRegisterICR(DutyCycleTimeValue);
    Timer::Timer1::enableFastPWM();
    Timer::Timer1::setPrescaler(Timer1Prescaler);
    Timer::Timer1::enableClearOnCompareMatchICR();
}

void moveServo() {
    if(currentDirection == DirectionIndex::Left){
        Timer::Timer1::setCompareRegisterByTime(Timer1Prescaler, ((int) Direction::Left) / 1000.0);
    }

    if(currentDirection == DirectionIndex::Right){
        Timer::Timer1::setCompareRegisterByTime(Timer1Prescaler, ((int) Direction::Right) / 1000.0);
    }

    if(currentDirection == DirectionIndex::Middle){
        Timer::Timer1::setCompareRegisterByTime(Timer1Prescaler, ((int) Direction::Middle) / 1000.0);
    }
}
