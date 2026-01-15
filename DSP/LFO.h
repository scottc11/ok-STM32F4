#pragma once

#include <math.h>
#include "common.h"
#include "Callback.h"
#include "utils.h"

// #define M_PI 3.14159265358979323846f // copied from math.h, not sure if I should define the preprocessor macros to make these available
#define M_HALF_PI (M_PI / 0.5f)
#define M_TWO_PI (2.0f * M_PI)

class LFO
{
public:
    
    enum Waveform
    {
        SINE = 0,
        SAW_RISING,
        SAW_FALLING
    };

    LFO()
    {
        frequency = 1.0f;
        phase = 0.0f;
        amplitude = 1.0f;
        sampleRate = 44100;
        halfComplete = false;
        waveform = SINE;
    }

    float value; // the most recently processed value of the waveform
    float frequency;
    float period;          // the time it takes for the waveform to complete one cycle
    float phase;           // position of the waveform within its cycle at a given point in time
    float phaseStepAmount; // the amount to increment the phase every sample
    float amplitude;       // the amplitude of the waveform (0.0 to 1.0)
    uint16_t sampleRate;   // usually the sample rate of the DAC
    bool halfComplete;     // flag to indicate if the waveform has completed half of its cycle
    Waveform waveform;  // currently selected waveform

    float minFrequency = 0.2f;
    float maxFrequency = 15.0f;
    float minAmplitude = 0.0f;
    float maxAmplitude = 1.0f;

    Callback<void()> fullCycleCallback;
    Callback<void()> halfCycleCallback;

    float update();
    uint16_t update_u16();
    void setSampleRate(uint16_t _sampleRate);
    void setFrequency(float _frequency);
    void setAmplitude(float _amplitude);
    void setWaveform(Waveform type);

    // helpers
    void handleFrequencyControl(uint16_t value);
    void handleAmplitudeControl(uint16_t value);

    float getValue();
    uint16_t getValue_u16();

    void setMinFrequency(float value);
    void setMaxFrequency(float value);
    void setMinAmplitude(float value);
    void setMaxAmplitude(float value);

    void attachFullCycleCallback(Callback<void()> callback)
    {
        fullCycleCallback = callback;
    }

    void attachHalfCycleCallback(Callback<void()> callback)
    {
        halfCycleCallback = callback;
    }
};