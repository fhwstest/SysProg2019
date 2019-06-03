#pragma once

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
