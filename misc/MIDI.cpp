#include "MIDI.h"

uint8_t MIDI::BUFFER_IN[3] = {0}; // static

void MIDI::sendNoteOn(uint8_t channel, uint8_t note, uint8_t velocity)
{
    uint8_t data[3] = {static_cast<uint8_t>(0x90 | channel), note, velocity};
    HAL_UART_Transmit(huart, data, 3, 1000);
};

void MIDI::sendNoteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
    uint8_t data[3] = {static_cast<uint8_t>(0x80 | channel), note, velocity};
    HAL_UART_Transmit(huart, data, 3, 1000);
};

uint8_t MIDI::getChannel(uint8_t status)
{
    return status & 0x0F;
}

void MIDI::resetDetectedBPM()
{
    lastClockTimestampUs = 0;
    clockIntervalAccumUs = 0;
    clockIntervalCount = 0;
    hasClockTimestamp = false;
    detectedBPM = 0.0f;
}

void MIDI::updateDetectedBPM(uint32_t timestampUs)
{
    // First tick only initializes the tracker.
    if (!hasClockTimestamp)
    {
        lastClockTimestampUs = timestampUs;
        hasClockTimestamp = true;
        return;
    }

    const uint32_t deltaUs = timestampUs - lastClockTimestampUs; // unsigned handles wrap-around
    lastClockTimestampUs = timestampUs;

    // Reject outliers and restart lock when transport pauses or ticks are too fast/noisy.
    if (deltaUs < 1000u || deltaUs > 500000u)
    {
        clockIntervalAccumUs = 0;
        clockIntervalCount = 0;
        return;
    }

    clockIntervalAccumUs += deltaUs;
    clockIntervalCount++;

    // 24 MIDI clocks = 1 quarter note.
    if (clockIntervalCount >= 24u)
    {
        const float avgDeltaUs = static_cast<float>(clockIntervalAccumUs) / static_cast<float>(clockIntervalCount);
        const float bpmRaw = 60000000.0f / (avgDeltaUs * 24.0f);

        // Smooth updates to reduce jitter while staying responsive.
        if (detectedBPM <= 0.0f)
        {
            detectedBPM = bpmRaw;
        }
        else
        {
            constexpr float alpha = 0.25f;
            detectedBPM += alpha * (bpmRaw - detectedBPM);
        }

        clockIntervalAccumUs = 0;
        clockIntervalCount = 0;
    }
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
    if (byte >= 0xF8)
    {
        // System realtime messages are 1-byte messages and can arrive at any time.
        BUFFER_IN[0] = byte;
        return true;
    }

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
    uint8_t status = data[0];
    uint8_t channel = getChannel(status);

    if (status >= 0xF8)
    {
        switch (status)
        {
        case MIDIstatus::CLOCK_START:
            resetDetectedBPM();
            if (clockStartCallback)
                clockStartCallback();
            break;

        case MIDIstatus::CLOCK_STOP:
            resetDetectedBPM();
            if (clockStopCallback)
                clockStopCallback();
            break;

        case MIDIstatus::CLOCK_CONTINUE:
            if (clockContinueCallback)
                clockContinueCallback();
            break;

        case MIDIstatus::CLOCK_TICK:
            if (clockTickCallback)
                clockTickCallback();
            break;

        default:
            break;
        }
        return;
    }

    uint8_t event = status & 0xF0;
    if (status >= 0x80)
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