#pragma once

#include <stddef.h>
#include <stdlib.h>

#include "ArrayIterator.h"

template<class T>
class DynamicArray {
public:
    explicit DynamicArray(size_t size) : _data((T*) malloc(sizeof(T) * size)),
                                         _size(size) {}

    ~DynamicArray() {
        free(_data);
    }

    DynamicArray(const DynamicArray<T> &other)
            : _data(malloc(sizeof(T) * other.size())),
              _size(other.size()) {
        for (size_t i = 0; i < _size; i++) {
            _data[i] = other._data[i];
        }
    }

    DynamicArray(DynamicArray<T> &&other) noexcept = default;

    constexpr size_t size() const {
        return _size;
    }

    constexpr T &operator[](size_t pos) {
        return _data[pos];
    }

    constexpr const T &operator[](size_t pos) const {
        return _data[pos];
    }

    ArrayIterator<T> begin() noexcept {
        return ArrayIterator<T>(_data, 0);
    };

    ArrayIterator<T> end() noexcept {
        return ArrayIterator<T>(_data, _size);
    };

    DynamicArray<T> &operator=(const DynamicArray<T> &other) {
        free(_data);

        _data = (T*) malloc(sizeof(T) * other.size());
        _size = other.size();

        for (size_t i = 0; i < _size; i++) {
            _data[i] = other._data[i];
        }

        return *this;
    }

    DynamicArray<T> &operator=(DynamicArray<T> &&other) noexcept = default;

private:
    T* _data;
    size_t _size;
};
