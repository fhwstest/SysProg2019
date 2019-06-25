#include <avr/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include <UART.h>
#include <Pins.h>
#include <StringBuilder.h>
#include <TempSensor.h>
#include <array/Array.h>
#include <CStringQueue.h>

enum class NewLine {
    None,
    RN,
    N
};

constexpr auto website = "<html><head><title>Hardlab Temperature</title><meta charset=\"utf-8\"/></head><body style=\"background-color: rgb(48, 45, 45)\"><h1 style=\"text-align:center;font-size:64px;color:#5ccadd\"> Temperature:</h1><p id=\"temp\" style=\"text-align: center; font-size:32px; color: rgb(241, 176, 79)\"></p> <script type=\"text/javascript\">(async()=>{t=document.getElementById('temp');fetch('/cTemp').then(x=>x.text()).then(x=>t.innerText=`${x} °C`);while(true){const x=await(fetch('/temp').then(x=>x.text()));t.innerText=`${x} °C`}})();</script> </body></html>";

using MyTempSensor = TempSensor<Port::C, 0>;

void sendMessageToClient(int id, const String &message, size_t statusCode = 200);

void sendCommandToClient(const String &command, NewLine newLine, const String &requiredResponse);

void enableTcpServer();

void initAccessPoint(const String &accessPointName);

void initPollingIds();

void sendTempToClients();

void closeConnection(int id);

StringBuilder<200, '\n'> strBuilder;
Array<int, 20> pollingIds;
int currentTemp = -1000;

CStringQueue responses;
CStringQueue messages;

void checkForMessage();

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

        checkForMessage();

        _delay_ms(1000);
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
    const String httpHeader = String("HTTP/1.1 ") + statusCode + "\n\n";

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

void sendCommandToClient(const String &command, NewLine newLine, const String &requiredResponse) {
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
                String response = responses.peek();
                responses.deleteFirst();

                if(response.find(requiredResponse) != -1) {
                    responseOK = true;
                }
            }

            _delay_ms(100);
        }
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
    if (messages.isEmpty()) {
        return;
    }

    String lastCommandStr = messages.peek();
    messages.deleteFirst();

    constexpr int idStart = 5;
    auto idEnd = (lastCommandStr.substr(idStart).find(","));
    int id = lastCommandStr.substr(idStart, (size_t) idEnd).to_intger();

    auto urlStart = lastCommandStr.find(" ") + 1;
    auto urlEnd = lastCommandStr.substr((size_t) urlStart).find(" ");
    auto url = lastCommandStr.substr((size_t) urlStart, (size_t) urlEnd);

    if (url == "/") {
        sendMessageToClient(id, website);
    } else if (url == "/temp") {
        for (int &i : pollingIds) {
            if (i == -1) {
                i = id;
                break;
            }
        }
    } else if (url == "/cTemp") {
        sendMessageToClient(id, currentTemp);
    } else {
        sendMessageToClient(id, "", 404);
    }

}

ISR(USART_RXC_vect) {
    const bool strComplete = strBuilder.add(UDR);

    if (strComplete) {
        String string = strBuilder.c_str();

        if (string.find("+IPD") != -1) {
            messages.push(string.c_str());
        } else if (string.find("OK") != -1){
            responses.push(string.c_str());
        }
    }
}