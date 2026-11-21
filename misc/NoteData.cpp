#include "NoteData.h"

float getNoteFrequency(Note note)
{
    return NOTE_DATA_MAP[note].frequency;
}

int16_t getNoteMidi(Note note)
{
    return NOTE_DATA_MAP[note].midi;
}

Note getNoteIndexFromMidi(uint8_t midi)
{
    // Index 0 is C-2. MIDI note 12 is C0, which is index 24.
    return static_cast<Note>(ok_clamp<uint8_t>(midi, 12, 127) + 12);
}