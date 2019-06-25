//
// Created by julian on 08.04.19.
//

#ifndef AVR_UART_H
#define AVR_UART_H

#include <stdint.h>
#include <stddef.h>

#include "String.h"

class UART {

public:
    UART() = delete;

    static void enableSync();
    static void enableAsync();

    static void setBaud(uint32_t baud);

    static void writeByte(uint8_t byte);
    static void writeString(const String &str);
    static void writeLineN(const String &str);
    static void writeLineRN(const String &str);
    static void writeBytesAsString(uint8_t* bytes, int bytesCount);
    static void printf(const char* format, ...);

    static uint8_t readByte();
    static void readNBytes(size_t size, uint8_t *buffer);
    static void readString(char* string, size_t maxStringSize);
    static void readLine(char *string, size_t maxStringSize);
    static String readString();
    static String readLine();

private:
    static constexpr uint16_t calcUbrr(uint32_t baud);

};


#endif //AVR_UART_H
