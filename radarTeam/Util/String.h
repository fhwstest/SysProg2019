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

    String(const char* str) {
        this->str = (char*) malloc(sizeof(char) * (strlen(str) + 1));
        strcpy(this->str, str);
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

    String& operator+= (const String& other) {
        char str[255];

        strcpy(str, this->c_str());
        strcat(str, other.c_str());

        free(this->str);
        this->str = (char*) malloc(sizeof(char) * (strlen(str) + 1));
        strcpy(this->str, str);

        return *this;
    }

    String& operator=(const String& other)
    {
        free(this->str);
        str = (char*)  malloc(sizeof(char) * (strlen(other.c_str()) + 1));
        strcpy(this->str, other.c_str());

        return *this;
    }

    String& operator=(String&& other) noexcept = default;

    bool operator==(const String& other)
    {
        return this->c_str() == other.c_str();
    }

    bool operator==(const char* other)
    {
        return this->c_str() == other;
    }

    bool operator!=(const String& other)
    {
        return this->c_str() != other.c_str();
    }

    bool operator!=(const char* other)
    {
        return this->c_str() != other;
    }

private:
    char* str;

};

inline String operator+ (const String& a, const String& b) {
    char str[255];

    strcpy(str, a.c_str());
    strcat(str, b.c_str());

    return String(str);
}

#endif //AVR_STRING_H
