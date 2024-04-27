#include "NoteData.h"

float getNoteFrequency(Note note)
{
    return NOTE_DATA_MAP[note].frequency;
}

uint8_t getNoteMidi(Note note)
{
    return NOTE_DATA_MAP[note].midi;
}