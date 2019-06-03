#pragma once

#include <stddef.h>

#include "ArrayIterator.h"

template<class T, size_t N>
class Array {
public:
    Array() = default;

    ~Array() = default;

    Array(const Array<T, N> &other) {
        *this = other;
    }

    Array(Array<T, N> &&other) noexcept = default;

    constexpr size_t size() const {
        return N;
    }

    T* data() {
        return _data;
    }

    const T* data() const {
        return _data;
    }

    constexpr T &operator[](size_t pos) {
        return _data[pos];
    }

    constexpr const T &operator[](size_t pos) const {
        return _data[pos];
    }

    ArrayIterator<T> begin() const noexcept {
        return ArrayIterator<T>(_data, 0);
    };

    ArrayIterator<T> end() const noexcept {
        return ArrayIterator<T>(_data, N);
    };

    Array<T, N> &operator=(const Array<T, N> &other) {
        for (size_t i = 0; i < N; i++) {
            _data[i] = other._data[i];
        }

        return *this;
    }

    Array<T, N> &operator=(Array<T, N> &&other) noexcept = default;

private:
    T _data[N];
};
