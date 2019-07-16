/*
 * Note.h
 *
 * Created: 15.07.2019 11:18:56
 *  Author: PC
 */ 


#ifndef NOTE_H_
#define NOTE_H_

typedef enum{
	c1 = 262,
	h = 247,
	a = 220,
	g = 196,
	f = 175,
	e = 165,
	d = 147,
	c = 131,
	pause = 0
} NoteFrequency;


typedef enum{
	ganz = 1000,
	halb = 500,
	viertel = 250,
	achtel = 125,
	sechzentel = 63
} NoteLength;


typedef struct{
	NoteFrequency freq;
	NoteLength len;
}Note;

#endif /* NOTE_H_ */