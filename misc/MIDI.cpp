#include "MIDI.h"

uint8_t MIDI::BUFFER_IN[3] = {0}; // static

void MIDI::sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
{
    uint8_t data[3] = {0x90 | channel, note, velocity};
};

void MIDI::sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
    uint8_t data[3] = {0x80 | channel, note, velocity};
};

uint8_t MIDI::getChannel(uint8_t status)
{
    return status & 0x0F;
}

/**
 * @brief Decides what to do with an incoming byte of MIDI data. 
 * Allows handling of different lengths of MIDI messages.
 * 
 * NOTE: This is likely executed in an ISR when a byte is received from the UART
 * 
 * @param byte 
 */
bool MIDI::processByte(uint8_t byte)
{
    static uint8_t index = 0;
    if (byte >= 0x80)
    {
        BUFFER_IN[0] = byte;
        index = 1;
    }
    else
    {
        BUFFER_IN[index] = byte;
        index++;
        if (index == 3)
        {
            index = 1; // this handles consecutive Note On events for the same MIDI channel occur without a corresponding Note Off event in between
            return true; // signal handler to process midi message
        }
    }
    return false; // midi message still incomplete
}

void MIDI::parseMessage(uint8_t *data)
{
    uint8_t channel = getChannel(data[0]);
    uint8_t event = data[0] & 0xF0;
    if (data[0] >= 0x80)
    {
        switch (event)
        {
        case MIDIstatus::NOTE_OFF:
            if (noteOffCallback)
                noteOffCallback(channel, data[1], data[2]);
            break;
        
        case MIDIstatus::NOTE_ON:
            if (noteOnCallback)
                noteOnCallback(channel, data[1], data[2]);
            break;

        case MIDIstatus::POLYPHONIC_KEY_PRESSURE:
            /* code */
            break;

        case MIDIstatus::CONTROL_CHANGE:
            /* code */
            break;

        case MIDIstatus::PROGRAM_CHANGE:
            /* code */
            break;
        
        case MIDIstatus::CHANNEL_PRESSURE:
            /* code */
            break;

        case MIDIstatus::PITCH_BEND:
            if (pitchBendCallback)
                pitchBendCallback(channel, (data[2] << 7) | data[1]);
            break;
            
        default:
            break;
        }
    }
};