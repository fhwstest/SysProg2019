#include <avr/delay.h>
#include <avr/interrupt.h>

#include <UART.h>
#include <Pins.h>
#include <StringBuilder.h>
#include <TempSensor.h>
#include <array/Array.h>

using MyTempSensor = TempSensor<Port::C, 0>;

void sendMessageToClient(int id, const String &message);

void sendCommandToClient(const String &command);

void enableTcpServer();

void initAccessPoint(const String &accessPointName);

void initPollingIds();

void sendTempToClients();

Array<int, 20> pollingIds;
int currentTemp = -1000;

int main() {
    UART::setBaud(9600);
    UART::enableAsync();

    sei();

    initAccessPoint("SSIDmitVerstand");
    enableTcpServer();

    initPollingIds();

    while (true) {
        int temp = static_cast<int>(MyTempSensor::readTemp());

        if (temp != currentTemp) {
            currentTemp = temp;
            sendTempToClients();
        }
    }
}

void sendTempToClients() {
    for (int &i : pollingIds) {
        if (i != -1) {
            sendMessageToClient(i, currentTemp);
            i = -1;
        }
    }
}

void initPollingIds() {
    for (int &i : pollingIds) {
        i = -1;
    }
}

void sendMessageToClient(int id, const String &message) {
    constexpr auto httpHeader = "HTTP/1.1 200 OK\n\n";

    const String httpMessage = httpHeader + message;

    String sendCommand = "AT+CIPSEND=";
    sendCommandToClient(sendCommand + id + "," + httpMessage.size());
    _delay_ms(200);
    sendCommandToClient(httpMessage);

    String closeCommand = "AT+CIPCLOSE=";
    sendCommandToClient(closeCommand + id);
}

void sendCommandToClient(const String &command) {
    UART::writeLineRN(command);
    _delay_ms(1000);
}

void enableTcpServer() {
    sendCommandToClient("AT+CIPMUX=1");
    sendCommandToClient("AT+CIPSERVER=1,80");
}

void initAccessPoint(const String &accessPointName) {
    sendCommandToClient("AT+CWMODE=3");
    sendCommandToClient("AT+CWSAP=\"" + accessPointName + "\",\"\",1,0");
}

StringBuilder<100, '\n'> strBuilder;

ISR(USART_RXC_vect) {
    const bool strComplete = strBuilder.add(UDR);

    if (strComplete) {
        String string = strBuilder.c_str();

        if (string.find("+IPD") != -1) {
            constexpr int idStart = 5;
            auto idEnd = (string.substr(idStart).find(","));
            int id = string.substr(idStart, (size_t) idEnd).to_intger();

            auto urlStart = string.find(" ") + 1;
            auto urlEnd = string.substr((size_t) urlStart).find(" ");
            auto url = string.substr((size_t) urlStart, (size_t) urlEnd);

            if (url == "/") {
                sendMessageToClient(id, "Hallo Welt");
            }

            if (url == "/temp") {
                for (int &i : pollingIds) {
                    if (i == -1) {
                        i = id;
                        break;
                    }
                }
            }

            if (url == "/cTemp") {
                sendMessageToClient(id, currentTemp);
            }
        }

        //TODO: Handle close
    }
}

