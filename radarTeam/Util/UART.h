//
// Created by julian on 08.04.19.
//

#ifndef AVR_UART_H
#define AVR_UART_H

#include <stdint.h>
#include <stddef.h>

#include "SString.h"

class UART {

public:
    UART() = delete;

    static void enableSync();
    static void enableAsync();

    static void setBaud(uint32_t baud);

    static void writeByte(uint8_t byte);

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

    static void writeBytesAsString(uint8_t* bytes, int bytesCount);

    static void printf(const char* format, ...);

    static uint8_t readByte();
    static void readNBytes(size_t size, uint8_t *buffer);
    static void readString(char* string, size_t maxStringSize);
    static void readLine(char *string, size_t maxStringSize);

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
    static constexpr uint16_t calcUbrr(uint32_t baud);

};


#endif //AVR_UART_H
