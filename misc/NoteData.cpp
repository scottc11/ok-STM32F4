#include "NoteData.h"

float getNoteFrequency(Note note)
{
    return NOTE_DATA_MAP[note].frequency;
}

uint8_t getNoteMidi(Note note)
{
    return NOTE_DATA_MAP[note].midi;
}

Note getNoteIndexFromMidi(uint8_t midi)
{
    return static_cast<Note>(ok_clamp<uint8_t>(midi, 12, 127) - 12);
}