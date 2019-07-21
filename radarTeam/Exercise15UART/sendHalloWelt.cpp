#include <Pins.h>
#include <UART.h>

int main() {
    UART::enableSync();
    UART::setBaud(9600);

    while (true) {
        UART::writeLineRN("Hallo Welt");
    }

}
