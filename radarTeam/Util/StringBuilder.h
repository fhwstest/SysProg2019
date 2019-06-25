//
// Created by julian on 03.06.19.
//

#ifndef AVR_STRINGBUILDER_H
#define AVR_STRINGBUILDER_H

template <size_t BufferSize, char Delimiter>
class StringBuilder {

public:
    bool add(char c) {
        if(c == '\r') {
            return false;
        }

        if(c != Delimiter && currentIndex != BufferSize-1) {
            buffer[currentIndex] = c;
            currentIndex++;

            complete = false;
        } else {
            buffer[currentIndex] = '\0';
            currentIndex = 0;

            complete = true;
        }

        return complete;
    }

    bool isComplete() const {
        return complete;
    }

    const char* c_str() const {
        return buffer;
    }

private:
    bool complete = false;
    size_t currentIndex = 0;
    char buffer[BufferSize];
};

#endif //AVR_STRINGBUILDER_H
