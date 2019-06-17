#include <UART.h>
#include <Pins.h>

void initAccesPoint(const String& accesPointName);

using LEDs = Pins<Port::B>;

String sendCommand(const String& command) {
    UART::writeLineRN(command.c_str());
    UART::readLine();

    return UART::readLine();
}

int main() {
    UART::setBaud(9600);
    UART::enableSync();

    LEDs::setAllOutput();
    LEDs::writeAllHigh();

    initAccesPoint("SSIDmitVerstand");
}

void initAccesPoint(const String& accesPointName) {
    sendCommand("AT+CWMODE=3");
    sendCommand("AT+CWSAP=\"" + accesPointName + "\",\"\",1,0");
}