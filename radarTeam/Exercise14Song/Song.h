//
// Created by Lars on 01.04.2019.
//

#ifndef AVR_SONG_H
#define AVR_SONG_H

#include "Note.h"

template<size_t MaxSongSize>
class Song {
public:
    void addNote(Note note, NoteLength length) {
        notes[size] = note;
        noteLengths[size] = length;

        ++size;
    }

    size_t size = 0;
    Note notes[MaxSongSize];
    NoteLength noteLengths[MaxSongSize];
};

#endif //AVR_SONG_H
