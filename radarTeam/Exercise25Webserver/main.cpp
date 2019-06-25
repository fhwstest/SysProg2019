#include <avr/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include <UART.h>
#include <Pins.h>
#include <StringBuilder.h>
#include <TempSensor.h>
#include <array/Array.h>

enum class NewLine {
    None,
    RN,
    N
};

constexpr auto website = "<!doctype html><head><meta charset=\"utf-8\"><title>HardTemp</title></head><body style=\"background-color: rgb(48, 45, 45)\"><h1 style=text-align:center;font-size:64px;color:#5ccadd> Temperature:</h1><p id=t style=text-align:center;font-size:32px;color:rgb(241, 176, 79)></p><script>(async()=>{t=document.getElementById('t');t.innerText=await fetch('/cTemp',{method:'get'})+' °C';while(true){t.innerText=await fetch('/temp',{method:'get'})+' °C';}})();</script></body></html>";

using MyTempSensor = TempSensor<Port::C, 0>;
using LEDs = Pins<Port::B>;

void sendMessageToClient(int id, const String &message, size_t statusCode = 200);

void sendCommandToClient(const String &command, NewLine newLine, const String& requiredResponse);

void enableTcpServer();

void initAccessPoint(const String &accessPointName);

void initPollingIds();

void sendTempToClients();

void closeConnection(int id);

StringBuilder<200, '\n'> strBuilder;
Array<int, 20> pollingIds;
int currentTemp = -1000;
char lastCommand[100];

void checkForMessage();

int main() {
    UART::setBaud(9600);
    UART::enableAsync();

    sei();

    LEDs::setAllOutput();
    LEDs::writeAllHigh();

    initAccessPoint("SSIDmitVerstand");
    enableTcpServer();

    initPollingIds();

    while (true) {
        int temp = static_cast<int>(MyTempSensor::readTemp());

        if (temp != currentTemp) {
            currentTemp = temp;
            sendTempToClients();
        }

        checkForMessage();
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

void sendMessageToClient(int id, const String &message, size_t statusCode) {
    const String httpHeader = String("HTTP/1.1 ")  + statusCode + "\n\n";

    int size = httpHeader.size() + message.size();

    String sendCommand = "AT+CIPSEND=";
    sendCommandToClient(sendCommand + id + "," + size, NewLine::RN, "OK");

    sendCommandToClient(httpHeader, NewLine::None, "");
    sendCommandToClient(message, NewLine::None, "SEND OK");

    closeConnection(id);
}

void closeConnection(int id) {
    String closeCommand = "AT+CIPCLOSE=";
    sendCommandToClient(closeCommand + id, NewLine::RN, "OK");
}

void sendCommandToClient(const String &command, NewLine newLine, const String& requiredResponse) {
    switch (newLine) {
        case NewLine::None:
            UART::writeString(command);
            break;
        case NewLine::RN:
            UART::writeLineRN(command);
            break;
        case NewLine::N:
            UART::writeLineN(command);
    }

    if(requiredResponse != "") {
        while (requiredResponse != lastCommand) {
            _delay_ms(1);
        }

        lastCommand[0] = '\0';
    }
}

void enableTcpServer() {
    sendCommandToClient("AT+CIPMUX=1", NewLine::RN, "OK");
    sendCommandToClient("AT+CIPSERVER=1,80", NewLine::RN, "OK");
}

void initAccessPoint(const String &accessPointName) {
    sendCommandToClient("AT+CWMODE=3", NewLine::RN, "OK");
    sendCommandToClient("AT+CWSAP=\"" + accessPointName + "\",\"\",1,0", NewLine::RN, "OK");
}

void checkForMessage() {
    String lastCommandStr = lastCommand;

    if (lastCommandStr.find("+IPD") != -1) {
        constexpr int idStart = 5;
        auto idEnd = (lastCommandStr.substr(idStart).find(","));
        int id = lastCommandStr.substr(idStart, (size_t) idEnd).to_intger();

        auto urlStart = lastCommandStr.find(" ") + 1;
        auto urlEnd = lastCommandStr.substr((size_t) urlStart).find(" ");
        auto url = lastCommandStr.substr((size_t) urlStart, (size_t) urlEnd);

        if (url == "/") {
            sendMessageToClient(id, website);
        }

        else if (url == "/temp") {
            for (int &i : pollingIds) {
                if (i == -1) {
                    i = id;
                    break;
                }
            }
        }

        else if (url == "/cTemp") {
            sendMessageToClient(id, currentTemp);
        }

        else {
            sendMessageToClient(id, "", 404);
        }

    }
}

ISR(USART_RXC_vect) {
    const bool strComplete = strBuilder.add(UDR);

    if (strComplete) {
        strcpy(lastCommand, strBuilder.c_str());
    }
}