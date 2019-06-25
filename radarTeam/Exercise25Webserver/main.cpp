#include <UART.h>
#include <Pins.h>

#include <avr/delay.h>
#include <avr/interrupt.h>

void readRequests();

using LEDs = Pins<Port::B>;

void sendCommand(const String &command) {
    UART::writeLineRN(command);

    /*
    String response;

    while (response.find("OK") == -1) {
        response = UART::readLine();
    }
     */

    _delay_ms(1000);
}

void enableTcpServer() {
    sendCommand("AT+CIPMUX=1");
    sendCommand("AT+CIPSERVER=1,80");
}

void initAccessPoint(const String &accessPointName) {
    sendCommand("AT+CWMODE=3");
    sendCommand("AT+CWSAP=\"" + accessPointName + "\",\"\",1,0");
}

int main() {
    UART::setBaud(9600);
    UART::enableSync();

    LEDs::setAllOutput();
    LEDs::writeAllHigh();

    initAccessPoint("SSIDmitVerstand");
    enableTcpServer();

    while (true) {
        readRequests();
    }
}

void readRequests() {
    String string = UART::readLine();

    if(string.find("+IPD") != -1) {
            constexpr int idStart = 5;
            auto idEnd = (string.substr(idStart).find(","));
            int id = string.substr(idStart, (size_t) idEnd).to_intger();

            auto urlStart = string.find(" ") + 1;
            auto urlEnd = string.substr((size_t) urlStart).find(" ");
            auto url = string.substr((size_t) urlStart, (size_t) urlEnd);

            if(url == "/") {
                String message = "Hallo Welt";

                String command = "AT+CIPSEND=";
                sendCommand(command + id + "," + message.size());
                sendCommand(message);
            }

            if(url == "/temp") {
                // TODO: implement
            }
        }
}

