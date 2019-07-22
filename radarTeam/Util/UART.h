//
// Created by julian on 08.04.19.
//

#ifndef AVR_UART_H
#define AVR_UART_H

#include <stdint.h>
#include <stddef.h>

#include <avr/io.h>

#include "SString.h"

class UART {

public:
    UART() = delete;

    static constexpr void enableSync() {
        //Enable sync UART
        UCSRB = (1 << RXEN) | (1 << TXEN);

        //Set Frame format: 8 bit data and 1 stop bits
        UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
    }

    static constexpr void enableAsync() {
        //Enable async UART
        UCSRB = (1 << RXEN) | (1 << TXEN) | (1 << RXCIE);

        //Set Frame format: 8 bit data and 1 stop bits
        UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
    }

    static constexpr void setBaud(uint32_t baud) {
        uint16_t ubrr = calcUbrr(baud);

        UBRRH = static_cast<uint8_t >(ubrr >> 8);
        UBRRL = static_cast<uint8_t>(ubrr);
    }

    static void writeByte(uint8_t byte) {
        // Wait for buffer to be empty
        while (!(UCSRA & (1 << UDRE)));

        UDR = byte;
    }

    template <size_t StringSize>
    static void writeString(const String<StringSize> &str) {
        size_t i = 0;

        while (str[i] != '\0') {
            writeByte(static_cast<uint8_t>(str[i++]));
        }
    }

    template <size_t StringSize>
    static void writeLineN(const String<StringSize> &str) {
        writeString(str);
        writeByte(static_cast<uint8_t>('\n'));
    }

    template <size_t StringSize>
    static void writeLineRN(const String<StringSize> &str) {
        writeString(str);
        writeByte(static_cast<uint8_t>('\r'));
        writeByte(static_cast<uint8_t>('\n'));
    }

    static void writeString(const char* str) {
        size_t i = 0;

        while (str[i] != '\0') {
            writeByte(static_cast<uint8_t>(str[i++]));
        }
    }

    static void writeLineN(const char* str) {
        writeString(str);
        writeByte(static_cast<uint8_t>('\n'));
    }

    static void writeLineRN(const char* str) {
        writeString(str);
        writeByte(static_cast<uint8_t>('\r'));
        writeByte(static_cast<uint8_t>('\n'));
    }

    static void writeBytesAsString(uint8_t* bytes, int bytesCount) {
        for (int i = 0; i < bytesCount; i++) {
            char str[4];
            sprintf(str, "%02X ", bytes[i]);
            UART::writeString(str);
        }

        UART::writeString("\r\n");
    }

    static void printf(const char* format, ...) {
        char buffer[256];

        va_list args;
        va_start (args, format);

        vsprintf (buffer,format, args);

        writeString(buffer);

        va_end (args);
    }

    static uint8_t readByte() {
        // Wait for data
        while (!(UCSRA & (1 << RXC)));

        return UDR;
    }

    static void readNBytes(size_t size, uint8_t *buffer) {
        for (size_t i = 0; i < size; ++i) {
            buffer[i] = readByte();
        }
    }

    static void readString(char* string, size_t maxStringSize) {
        for (size_t i = 0; i < maxStringSize; ++i) {
            string[i] = static_cast<char>(readByte());

            if (string[i] == '\0') {
                return;
            }
        }
    }

    static void readLine(char *string, size_t maxStringSize) {
        for (size_t i = 0; i < maxStringSize; ++i) {
            string[i] = static_cast<char>(readByte());

            if (string[i] == '\n') {
                if(i != 0 && string[i-1] == '\r') {
                    string[i-1] = '\0';
                } else {
                    string[i] = '\0';
                }

                return;
            }
        }

        string[maxStringSize] = '\0';
    }

    template <size_t StringSize>
    static String<StringSize> readString() {
        char str[StringSize];
        readString(str, StringSize - 1);

        return String(str);
    }

    template <size_t StringSize>
    static String<StringSize> readLine() {
        char str[StringSize];
        readLine(str, StringSize - 1);

        return String(str);
    }

private:
    static constexpr uint16_t calcUbrr(uint32_t baud) {
        return static_cast<uint16_t>(F_CPU / (static_cast<uint32_t >(16) * baud) - 1);
    }

};


#endif //AVR_UART_H
