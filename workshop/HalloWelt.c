#include <avr/io.h>
#include <stdbool.h>

#include "uart.h"

int main() {
	DDRC = 0xff;
	DDRA = 0;

	PORTC = 0xff;

	usart_init();

	printf("Hallo Welt\n");

	while(true) {
		PORTC = PINA;

	}

	return 0;
}
