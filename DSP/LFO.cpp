#include "LFO.h"

/**
 * @brief advance the LFO waveform by one sample and return the current value
 * 
 * @return float 
 */
float LFO::update()
{
    // the frequency of the waveform is determined by the size of the steps.
    phase += phaseStepAmount;

    if (phase >= M_TWO_PI)
    {
        phase -= M_TWO_PI;
        halfComplete = false;
        if (fullCycleCallback)
            fullCycleCallback();
    }
    else if (phase >= M_PI && !halfComplete)
    {
        halfComplete = true;
        if (halfCycleCallback)
            halfCycleCallback();
    }

    switch (waveform)
    {
    case Waveform::SAW_RISING:
    {
        // Map phase (0..2π) to a -1..1 rising saw
        float normalizedPhase = 1.0f - (phase / M_TWO_PI);  // 1..0
        value = ((2.0f * normalizedPhase) - 1.0f) * amplitude;
        break;
    }
    case Waveform::SAW_FALLING:
    {
        // Map phase (0..2π) to a -1..1 rising saw
        float normalizedPhase = phase / M_TWO_PI; // 0..1
        value = ((2.0f * normalizedPhase) - 1.0f) * amplitude;
        break;
    }
    case Waveform::SINE:
    default:
        value = arm_sin_f32(phase) * amplitude; // note: try using the arm_sin_f32.c
        break;
    }
    return value;
}

/**
 * @brief advance the LFO waveform by one sample and return a 16-bit value
 * 
 * @return uint16_t 
 */
uint16_t LFO::update_u16()
{
    return ok_float_to_u16(update(), -1.0f, 1.0f); // see arm_q15_to_float.c
}


float LFO::getValue()
{
    return value;
}

/**
 * @brief get the current value of the LFO waveform as a 16-bit value
 * 
 * @return uint16_t 
 */
uint16_t LFO::getValue_u16()
{
    return ok_float_to_u16(value, -1.0f, 1.0f); // see arm_q15_to_float.c
}

void LFO::setSampleRate(uint16_t _sampleRate)
{
    sampleRate = _sampleRate;
    this->setFrequency(frequency);
}

void LFO::setFrequency(float _frequency)
{
    frequency = _frequency;
    period = M_TWO_PI * frequency;
    phaseStepAmount = period * (1.0f / sampleRate);
}

void LFO::setAmplitude(float _amplitude)
{
    amplitude = ok_clamp<float>(_amplitude, minAmplitude, maxAmplitude);
}

void LFO::setWaveform(Waveform type)
{
    waveform = type;
}

/**
 * @brief Set the frequency of the LFO based on a 12-bit value
 * 
 * @param value 
 * @return float 
 */
void LFO::handleFrequencyControl(uint16_t value)
{
    this->setFrequency(ok_map<float>((float)value, 0.0f, 4095.0f, minFrequency, maxFrequency));
}

/**
 * @brief Set the amplitude of the LFO based on a 12-bit value
 * 
 * @param value 
 * @return float 
 */
void LFO::handleAmplitudeControl(uint16_t value)
{
    this->setAmplitude(ok_map<float>((float)value, 0.0f, 4095.0f, minAmplitude, maxAmplitude));
}

void LFO::setMinFrequency(float value)
{
    minFrequency = value;
}

void LFO::setMaxFrequency(float value)
{
    maxFrequency = value;
}

void LFO::setMinAmplitude(float value) {
    minAmplitude = value;
}

void LFO::setMaxAmplitude(float value) {
    maxAmplitude = value;
}