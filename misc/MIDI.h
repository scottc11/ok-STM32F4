#pragma once

#include <stdint.h>
#include "Callback.h"

enum MIDIstatus
{
    NOTE_OFF = 0x80,
    NOTE_ON = 0x90,
    POLYPHONIC_KEY_PRESSURE = 0xA0, // This message is most often sent by pressing down on the key after it “bottoms out”.
    CONTROL_CHANGE = 0xB0,          // This message is sent when a controller value changes. Controllers include devices such as pedals and levers. Controller numbers 120-127 are reserved as “Channel Mode Messages” (below). (ccccccc) is the controller number (0-119). (vvvvvvv) is the controller value (0-127).
    PROGRAM_CHANGE = 0xC0,          // This message sent when the patch number changes. (ppppppp) is the new program number.
    CHANNEL_PRESSURE = 0xD0,        // This message is most often sent by pressing down on the key after it “bottoms out”. This message is different from polyphonic after-touch. Use this message to send the single greatest pressure value (of all the current depressed keys).
    PITCH_BEND = 0xE0               // This message is sent to indicate a change in the pitch bender (wheel or lever, typically). The pitch bender is measured by a fourteen bit value. Center (no pitch change) is 2000H. Sensitivity is a function of the receiver, but may be set using RPN 0. (lllllll) are the least significant 7 bits. (mmmmmmm) are the most significant 7 bits.
};

class MIDI {
    public:
        MIDI() {}

        void sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity);
        void sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity);

        bool processByte(uint8_t byte);
        void parseMessage(uint8_t *data);
        
        void attachNoteOnCallback(Callback<void(uint8_t channel, uint8_t note, uint8_t velocity)> callback) { noteOnCallback = callback; }
        void attachNoteOffCallback(Callback<void(uint8_t channel, uint8_t note, uint8_t velocity)> callback) { noteOffCallback = callback; }
        void attachPitchBendCallback(Callback<void(uint8_t channel, uint16_t pitch)> callback) { pitchBendCallback = callback; }

        static uint8_t BUFFER_IN[3];

    private:
        Callback<void(uint8_t channel, uint8_t note, uint8_t velocity)> noteOnCallback;
        Callback<void(uint8_t channel, uint8_t note, uint8_t velocity)> noteOffCallback;
        Callback<void(uint8_t channel, uint16_t pitch)> pitchBendCallback;
        
        uint8_t getChannel(uint8_t status);
};