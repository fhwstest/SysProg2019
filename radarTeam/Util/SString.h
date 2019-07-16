//
// Created by julian on 17.06.19.
//

#ifndef AVR_SSTRING_H
#define AVR_SSTRING_H

#include <string.h>
#include <stdlib.h>

template <size_t Size>
class String {

public:
    String() = default;

    String(const char (& cString)[Size]) {
        strcpy(this->cString, cString);
    }

    String(int value) {
        itoa(value, cString, 10);
    }

    const char* c_str() const {
        return cString;
    }

    size_t size() const {
        return strlen(cString);
    }

    template <size_t OtherSize>
    int32_t find(const String<OtherSize>& str, size_t start = 0) const {

        const char* pos = strstr(c_str() + start, str.c_str());

        if(pos == nullptr) {
            return -1;
        }

        return pos - c_str();
    }

    int32_t find(const char* str, size_t start = 0) const {

        const char* pos = strstr(c_str() + start, str);

        if(pos == nullptr) {
            return -1;
        }

        return pos - c_str();
    }

    template <size_t NewSize>
    String<NewSize> substr(size_t index, size_t size = NewSize) const {
        String<NewSize> newString{};

        char* buffer = const_cast<char*>(newString.c_str());

        strncpy(buffer, cString, size);

        return newString;
    }

    int to_intger() const {
        return atoi(cString);
    }

    template <size_t OtherSize>
    bool operator==(const String<OtherSize>& other) const
    {
        return strcmp(this->c_str(), other.c_str()) == 0;
    }

    bool operator==(const char* other) const
    {
        return strcmp(this->c_str(), other) == 0;
    }

    template <size_t OtherSize>
    bool operator!=(const String<OtherSize>& other) const
    {
        return !(*this == other);
    }

    bool operator!=(const char* other) const
    {
        return !(*this == other);
    }

    constexpr char operator[](size_t index) const {
        return cString[index];
    }

private:
    char cString[Size]{};

};

template <size_t SizeA, size_t SizeB>
String<SizeA + SizeB + 1> operator+(const String<SizeA>& a, const String<SizeB>& b) {
    String<SizeA + SizeB + 1> newString;

    char* buffer = const_cast<char*>(newString.c_str());

    strcpy(buffer, a.c_str());
    strcat(buffer, b.c_str());

    return newString;
}

template <size_t SizeA, size_t SizeB>
String<SizeA + SizeB + 1> operator+(const char (& a)[SizeA], const String<SizeB>& b) {
    String<SizeA + SizeB + 1> newString;

    char* buffer = const_cast<char*>(newString.c_str());

    strcpy(buffer, a);
    strcat(buffer, b.c_str());

    return newString;
}

template <size_t SizeA, size_t SizeB>
String<SizeA + SizeB + 1> operator+(const String<SizeA>& a, const char (& b)[SizeB]) {
    String<SizeA + SizeB + 1> newString;

    char* buffer = const_cast<char*>(newString.c_str());

    strcpy(buffer, a.c_str());
    strcat(buffer, b);

    return newString;
}


#endif //AVR_SSTRING_H
