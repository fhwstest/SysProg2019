#include <avr/interrupt.h>

#include <UART.h>

int main() {
    UART::enableAsync();
    UART::setBaud(9600);

    sei();

    while (true);
}

ISR(USART_RXC_vect) {
    UART::writeByte(UDR);
}