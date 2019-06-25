//
// Created by julian on 25.06.19.
//

#ifndef AVR_CSTRINGQUEUE_H
#define AVR_CSTRINGQUEUE_H

#include <string.h>
#include <stdlib.h>

class CStringQueueNode {
public:
    char* value;
    CStringQueueNode* next;
};

class CStringQueue {
public:
    void push(const char* str) {
        CStringQueueNode* lastNode = nullptr;

        if(start == nullptr) {
            start = static_cast<CStringQueueNode*>(malloc(sizeof(CStringQueueNode)));
            start->next = nullptr;

            lastNode = start;
        } else {
            CStringQueueNode* currentNode = start;

            while (currentNode->next != nullptr) {
                currentNode = currentNode->next;
            }

            currentNode->next = static_cast<CStringQueueNode*>(malloc(sizeof(CStringQueueNode)));
            currentNode->next->next = nullptr;

            lastNode = currentNode->next;
        }

        lastNode->value = static_cast<char *>(malloc(sizeof(char) * (strlen(str) + 1)));
        strcpy(lastNode->value, str);
    }

    void deleteFirst() {
        auto cStart = start;

        start = start->next;

        free(cStart->value);
        free(cStart);
    }

    void pop(char* str) {
        strcpy(str, start->value);

        deleteFirst();
    }

    const char* peek() {
        return  start->value;
    }

    bool isEmpty() const {
        return start == nullptr;
    }

    size_t size() const {
        size_t count = 0;

        auto current = start;

        while (current != nullptr) {
            count++;
            current = current->next;
        }

        return count;
    }

private:
    CStringQueueNode* start;
};

#endif //AVR_CSTRINGQUEUE_H
