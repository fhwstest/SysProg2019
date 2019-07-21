//
// Created by JulZi on 21.07.2019.
//

#ifndef AVR_DIRECTION_H
#define AVR_DIRECTION_H

// Constants

constexpr int SERVO_MIN = 815;
constexpr int SERVO_MAX = 2000;

// Enums

enum class DirectionIndex {
    Left = 'a',
    Right = 'b',
    Middle = 'c'
};

enum class Direction {
    Left = 960,
    Right = 2164,
    Middle = 1600
};


#endif //AVR_DIRECTION_H
