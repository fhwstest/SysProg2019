//
// Created by Lars on 01.04.2019.
//

#ifndef AVR_NOTE_H
#define AVR_NOTE_H

enum class Note {
    c1 = 523,
    h = 493 ,
    a = 440,
    g = 391,
    f = 349,
    e = 329,
    d = 293,
    c = 261,
    pause = 0
};

enum class NoteLength {
    ganz = 1000,
    halb = ganz / 2,
    viertel = halb / 2,
    achtel = viertel / 2,
    sechzentel = achtel / 2
};

#endif //AVR_NOTE_H
