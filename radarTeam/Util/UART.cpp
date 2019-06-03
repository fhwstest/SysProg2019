//
// Created by julian on 08.04.19.
//

#include "UART.h"

#include <avr/io.h>

#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>

void UART::enableSync() {
    //Enable sync UART
    UCSRB = (1 << RXEN) | (1 << TXEN);

    //Set Frame format: 8 bit data and 2 stop bits
    UCSRC = (1 << URSEL) | (1 << USBS) | (3 << UCSZ0);
}

void UART::enableAsync() {
    //Enable async UART
    UCSRB = (1 << RXEN) | (1 << TXEN) | (1 << RXCIE);

    //Set Frame format: 8 bit data and 1 stop bits
    UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
}

void UART::writeByte(uint8_t byte) {
    // Wait for buffer to be empty
    while (!(UCSRA & (1 << UDRE)));

    UDR = byte;
}

void UART::writeString(const char *str) {
    size_t i = 0;

    while (str[i] != '\0') {
        writeByte(static_cast<uint8_t>(str[i++]));
    }
}

void UART::writeBytesAsString(uint8_t *bytes, int bytesCount) {
    for (int i = 0; i < bytesCount; i++) {
        char str[4];
        sprintf(str, "%02X ", bytes[i]);
        UART::writeString(str);
    }

    UART::writeString("\r\n");
}

void UART::printf(const char *format, ...) {
    char buffer[256];

    va_list args;
    va_start (args, format);

    vsprintf (buffer,format, args);

    writeString(buffer);

    va_end (args);
}

uint8_t UART::readByte() {
    // Wait for data
    while (!(UCSRA & (1 << RXC)));

    return UDR;
}

void UART::readNBytes(size_t size, uint8_t *buffer) {
    for (size_t i = 0; i < size; ++i) {
        buffer[i] = readByte();
    }
}

void UART::readString(char *string, size_t maxStringSize) {
    for (size_t i = 0; i < maxStringSize; ++i) {
        string[i] = static_cast<char>(readByte());

        if (string[i] == '\0') {
            return;
        }
    }
}

void UART::readLine(char *string, size_t maxStringSize) {
    for (size_t i = 0; i < maxStringSize; ++i) {
        string[i] = static_cast<char>(readByte());

        if (string[i] == '\n') {
            string[i] = '\0';
            return;
        }
    }
}

void UART::setBaud(uint16_t baud) {
    uint16_t ubrr = calcUbrr(baud);

    UBRRH = static_cast<uint8_t >(ubrr >> 8);
    UBRRL = static_cast<uint8_t>(ubrr);
}

constexpr uint16_t UART::calcUbrr(uint16_t baud) {
    return SYSTEM_CLOCK / (static_cast<uint32_t >(16) * baud) - 1;
}
