//
// Created by julian on 03.06.19.
//

#ifndef AVR_STRINGBUILDER_H
#define AVR_STRINGBUILDER_H

template <size_t BufferSize, char Delimiter>
class StringBuilder {

public:
    bool add(char c) {
        if(c != Delimiter) {
            buffer[currentIndex] = c;
            currentIndex++;

            return false;
        } else {
            buffer[currentIndex] = '\0';
            currentIndex = 0;

            return true;
        }
    }

    const char* c_str() const {
        return buffer;
    }

private:
    size_t currentIndex = 0;
    char buffer[BufferSize];
};

#endif //AVR_STRINGBUILDER_H
