#include <stdio.h>

#include <UART.h>
#include <TempSensor.h>

constexpr Port OneWirePort = Port::C;
constexpr uint8_t OneWirePin = 0;

using MyTempSensor = TempSensor<OneWirePort, OneWirePin>;

int main() {
    UART::enableSync();
    UART::setBaud(9600);

    int currentTemp = -1000;

    while (true) {
        int temp = static_cast<int>(MyTempSensor::readTemp());

        if(temp != currentTemp) {
            currentTemp = temp;
            UART::printf("Temp: %d\n", currentTemp);
        }
    }
}