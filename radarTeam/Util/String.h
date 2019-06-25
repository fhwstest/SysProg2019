//
// Created by julian on 17.06.19.
//

#ifndef AVR_STRING_H
#define AVR_STRING_H

#include <string.h>
#include <stdlib.h>

class String {

public:
     String() {
        str = (char*) malloc(sizeof(char));
        str[0] = '\0';
    }

    String(int value) {
        char str[10];
        itoa(value, str, 10);

        this->str = (char*) malloc(sizeof(char) * (strlen(str) + 1));
        strcpy(this->str, str);
    }

    String(const char* str) {
        this->str = (char*) malloc(sizeof(char) * (strlen(str) + 1));
        strcpy(this->str, str);
    }

    String(const char* str, size_t length) {
        this->str = (char*) malloc(sizeof(char) * (length + 1));
        memcpy(this->str, str, length);
        this->str[length] = '\0';
    }

    String(const String& other) : String(other.c_str()) {
    }

    String(String&& other) noexcept = default;

    ~String() {
        free(str);
    }

    size_t size() const {
        return strlen(str);
    }

    const char* c_str() const {
        return str;
    }

    int32_t find(const String& str) const {

        char* pos = strstr(c_str(), str.c_str());

        if(pos == nullptr) {
            return -1;
        }

        return pos - c_str();
    }

    int to_intger() const {
        return atoi(str);
    }

    String substr(size_t start) const {
        return str + start;
    }

    String substr(size_t start, size_t length) const {
        return String(str + start, length);
    }

    String& operator+= (const String& other) {
        size_t length = size() + other.size() + 1;
        char* buffer = (char*) malloc(sizeof(char) * length);

        strcpy(buffer, this->c_str());
        strcat(buffer, other.c_str());

        free(str);

        str = (char*) malloc(sizeof(char) * length);
        strcpy(str, buffer);

        free(buffer);

        return *this;
    }

    String& operator=(const String& other)
    {
        free(str);
        str = (char*)  malloc(sizeof(char) * (other.size() + 1));
        strcpy(str, other.c_str());

        return *this;
    }

    String& operator=(String&& other) noexcept = default;

    bool operator==(const String& other) const
    {
        return strcmp(this->c_str(), other.c_str()) == 0;
    }

    bool operator==(const char* other) const
    {
        return strcmp(this->c_str(), other) == 0;
    }

    bool operator!=(const String& other) const
    {
        return !(*this == other);
    }

    bool operator!=(const char* other) const
    {
        return !(*this == other);
    }

    char operator[](size_t index) const {
        return str[index];
    }

private:
    char* str;

};

inline String operator+ (const String& a, const String& b) {
    size_t length = a.size() + b.size() + 1;

    char* buffer = (char*) malloc(sizeof(char) * length);

    strcpy(buffer, a.c_str());
    strcat(buffer, b.c_str());

    String str = String(buffer);

    free(buffer);

    return str;
}

#endif //AVR_STRING_H
