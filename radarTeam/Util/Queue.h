#pragma once

#include <stdint.h>

#include "Array.h"

template<typename T, size_t N>
class Queue {

public:
    bool push(const T& value) {
        if(currentWrite != currentRead || !lastWasInsert) {
            lastWasInsert = true;
            array[currentWrite] = value;
            currentWrite = (currentWrite + 1) % N;
            return true;
        }

        return false;
    }

    T pop() {
        lastWasInsert = false;
        size_t oldRead = currentRead;
        currentRead = (currentRead + 1) % N;
        return array[oldRead];
    }

    T& peek() {
        return array[currentRead];
    }

    bool isEmpty() const {
        return size() == 0;
    }

    size_t size() const {
        if(currentWrite > currentRead) {
            return currentWrite - currentRead;
        }

        if(currentWrite == currentRead && !lastWasInsert) {
            return 0;
        }

        return N - currentRead + currentWrite;
    }

private:
    bool lastWasInsert = false;
    size_t currentWrite = 0;
    size_t currentRead = 0;
    Array<T, N> array;

};