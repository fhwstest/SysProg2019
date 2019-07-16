#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include <UART.h>
#include <Pins.h>
#include <StringBuilder.h>
#include <TempSensor.h>
#include <Array.h>
#include <Queue.h>

#include "NewLine.h"

// Forward declarations

void initAccessPoint();
void enableTcpServer();
void checkForTemperatureChange();
void checkForRequest();
template <size_t StingSize> void sendMessageToClient(int id, const String<StingSize>& message, size_t statusCode = 200);
template <size_t StingSize> void sendCommandToClient(const String<StingSize>& command, NewLine newLine, const char* requiredResponse);

// Website data

const String website = "<html><head><title>Hardlab Temperature</title>"
                 "<meta charset=\"utf-8\"/></head><body "
                 "style=\"background-color: rgb(48, 45, 45)\""
                 "><h1 style=\"text-align:center;font-size:64px;color"
                 ":#5ccadd\"> Temperature:</h1><p id=\"temp\" style=\"t"
                 "ext-align: center; font-size:32px; color: rgb(241, 176, 79)\""
                 "></p> <script type=\"text/javascript\">(async()=>{t=document.getEle"
                 "mentById('temp');fetch('/cTemp').then(x=>x.text()).then(x=>t.innerText=`"
                 "${x} °C`);while(true){const x=await(fetch('/temp').then(x=>x.text()));t."
                 "innerText=`${x} °C`}})();</script> </body></html>";

// Constants

const String SSID = "SSIDmitVerstand";

constexpr size_t MaxClients = 20;
constexpr size_t MaxRequestLineSize = 255;
constexpr size_t MaxRequests = 10;
constexpr size_t MaxResponses = 10;

constexpr size_t MaxUrlStringSize = 30;
constexpr size_t MaxIdStringSize = 4;
constexpr size_t MaxTempStringSize = 5;
constexpr size_t MaxStatusCodeStringSize = 5;
constexpr size_t MaxByteSizeStringSize = 5;

// Port and Pin declarations

using MyTempSensor = TempSensor<Port::C, 0>;

// Object declarations

Queue<int, MaxClients> pollingIds;
int currentTemp;

StringBuilder<MaxRequestLineSize, '\n'> strBuilder;
Queue<String<MaxRequestLineSize>, MaxResponses> responses;
Queue<String<MaxRequestLineSize>, MaxRequests> requests;

// Functions

int main() {
    UART::setBaud(9600);
    UART::enableAsync();

    sei();

    initAccessPoint();
    enableTcpServer();

    while (true) {
        checkForTemperatureChange();
        checkForRequest();

        _delay_ms(1000);
    }
}

ISR(USART_RXC_vect) {
    const bool strComplete = strBuilder.add(UDR);

    if (strComplete) {
        auto string = strBuilder.get();

        if (string.find("+IPD") != -1) {
            requests.push(string);
        } else if (string.find("OK") != -1){
            responses.push(string);
        }
    }
}

void initAccessPoint() {
    sendCommandToClient(String("AT+CWMODE=3"), NewLine::RN, "OK");
    sendCommandToClient(String("AT+CWSAP=\"") + SSID + "\",\"\",1,0", NewLine::RN, "OK");
}

void enableTcpServer() {
    sendCommandToClient(String("AT+CIPMUX=1"), NewLine::RN, "OK");
    sendCommandToClient(String("AT+CIPSERVER=1,80"), NewLine::RN, "OK");
}

void checkForTemperatureChange() {
    int temp = static_cast<int>(MyTempSensor::readTemp());

    if (temp != currentTemp) {
        currentTemp = temp;

        while(!pollingIds.isEmpty()) {
            sendMessageToClient(pollingIds.pop(), String<MaxTempStringSize>(currentTemp));
        }
    }
}

void checkForRequest() {
    if (requests.isEmpty()) {
        return;
    }

    auto request = requests.pop();

    constexpr int idStart = 5;
    auto idEnd = request.find(",", idStart + 1);
    int id = request.substr<MaxIdStringSize>(idStart, idEnd).to_intger();

    auto urlStart = request.find(" ") + 1;
    auto urlEnd = request.find(" ", urlStart + 1);
    auto url = request.substr<MaxUrlStringSize>((size_t) urlStart, urlEnd);

    if (url == "/") {
        sendMessageToClient(id, website);
    } else if (url == "/temp") {
        pollingIds.push(id);
    } else if (url == "/cTemp") {
        sendMessageToClient(id, String<MaxTempStringSize>(currentTemp));
    } else {
        sendMessageToClient(id, String(""), 404);
    }
}

template <size_t StingSize>
void sendMessageToClient(int id ,const String<StingSize>& message, size_t statusCode) {
    auto httpHeader = "HTTP/1.1 " + String<MaxStatusCodeStringSize>(statusCode) + "\n\n";
    int size = httpHeader.size() + message.size();

    auto sendCommand = "AT+CIPSEND=" + String<MaxIdStringSize>(id) + "," + String<MaxByteSizeStringSize>(size);
    sendCommandToClient(sendCommand, NewLine::RN, "OK");

    sendCommandToClient(httpHeader, NewLine::None, "");
    sendCommandToClient(message, NewLine::None, "SEND OK");

    auto closeCommand = "AT+CIPCLOSE=" + String<MaxIdStringSize>(id);
    sendCommandToClient(closeCommand, NewLine::RN, "OK");
}

template <size_t StingSize>
void sendCommandToClient(const String<StingSize>& command, NewLine newLine, const char* requiredResponse) {
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

    if (requiredResponse != "") {
        bool responseOK = false;

        while (!responseOK) {
            if(!responses.isEmpty()) {
                auto response = responses.pop();

                if(response.find(requiredResponse) != -1) {
                    responseOK = true;
                }
            }

            _delay_ms(100);
        }
    }
}
