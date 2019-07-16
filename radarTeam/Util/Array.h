#pragma once

#include <stddef.h>

template<class T>
class ArrayIterator {
public:
    explicit ArrayIterator(const T* array, size_t i) : array(array), i(i) {}

    void operator++() {
        i++;
    }

    const T &operator*() const {
        return array[i];
    }

    T &operator*() {
        return const_cast<T&>(array[i]);
    }

    bool operator!=(const ArrayIterator<T> &other) const {
        if(array == other.array) {
            return i != other.i;
        }

        return true;
    }

private:
    size_t i;
    const T* array;
};

template<class T, size_t N>
class Array {
public:
    Array() = default;

    explicit Array(const T& defaultValue) {
        for(T& value : *this) {
            value = defaultValue;
        }
    }

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

private:
    T _data[N];
};
