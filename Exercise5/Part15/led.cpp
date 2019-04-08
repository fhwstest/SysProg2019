#include <UART.h>
#include <Pins.h>

using LEDs = Pins<Port::C>;
using Buttons = Pins<Port::A>;

int main() {
    LEDs::setAllOutput();
    Buttons::setAllInput();

    // Disable leds. Default is on
    LEDs::writeAllHigh();

    UART::setBaud(9600);
    UART::enableSync();

    while(true) {
        uint8_t ledMask = UART::readByte();
        LEDs::writeMask(ledMask);

        uint8_t buttonMask = Buttons::readAll<PortType::Pin>();
        UART::writeByte(buttonMask);
    }

}