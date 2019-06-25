//
// Created by julian on 25.06.19.
//

#ifndef AVR_QUEUE_H
#define AVR_QUEUE_H

#include <stdlib.h>

template <typename T>
class QueueNode {
public:
    T value;
    QueueNode<T>* next;
};

template <typename T>
class Queue {

public:
    void push(const T& value) {
        if(start == nullptr) {
            start = static_cast<QueueNode<T> *>(malloc(sizeof(QueueNode<T>)));
            start->next = nullptr;
            start->value = value;

            return;
        }

        QueueNode<T>* currentNode = start;

        while (currentNode->next != nullptr) {
            currentNode = currentNode->next;
        }

        currentNode->next = static_cast<QueueNode<T> *>(malloc(sizeof(QueueNode<T>)));
        currentNode->next->next = nullptr;
        currentNode->next->value = value;
    }

    T pop() {
        QueueNode<T>* cStart = start;

        start = start->next;

        auto value = cStart->value;

        cStart->value.~T();
        free(cStart);

        return value;
    }

    T& peek() {
        return  start->value;
    }

    bool isEmpty() const {
        return start == nullptr;
    }

    size_t size() const {
        size_t count = 0;

        QueueNode<T>* current = start;

        while (current != nullptr) {
            count++;
            current = current->next;
        }

        return count;
    }

private:
    QueueNode<T>* start;

};

#endif //AVR_QUEUE_H
