#include "LFO.h"

/**
 * @brief reset the LFO waveform to the beginning of the cycle
 */
void LFO::reset()
{
    phase = 0.0f;
    halfComplete = false;
}

/**
 * @brief trigger the sample hold waveform with a 16-bit sample value
 * 
 * @param sample 16-bit sample value
 */
void LFO::triggerSampleHold(uint16_t sample)
{
    if (waveform != Waveform::SAMPLE_HOLD)
    {
        return;
    }
    const float t = static_cast<float>(sample) / 65535.0f;
    value = ((t * 2.0f) - 1.0f) * amplitude;
}

/**
 * @brief advance the LFO waveform by one sample and return the current value
 * 
 * @return float 
 */
float LFO::update()
{
    if (waveform == Waveform::SAMPLE_HOLD)
    {
        return value;
    }
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
 * @brief Set the frequency range of the LFO waveform
 * 
 * @param range 
 */
void LFO::setFrequencyRange(Range range)
{
    this->range = range;
    switch (range)
    {
    case Range::SLOW:
        minFrequency = 0.02f;
        maxFrequency = 1.5f;
        break;
    case Range::MEDIUM:
        minFrequency = 0.2f;
        maxFrequency = 15.0f;
        break;
    case Range::FAST:
        minFrequency = 2.0f;
        maxFrequency = 150.0f;
        break;
    default:
        break;
    }
}

/**
 * @brief Set the frequency of the LFO based on a 12-bit value
 * 
 * @param value 
 * @return float 
 */
void LFO::handleFrequencyControl(uint16_t value)
{
    if (tempoSync)
    {
        const uint8_t syncRateCount = static_cast<uint8_t>(LFO::SyncRate::SIXTEENTH) + 1u;
        const uint8_t syncRateIndex = static_cast<uint8_t>((value * syncRateCount) / 4096u);
        setSyncRate(static_cast<LFO::SyncRate>(syncRateIndex));
    } else {
        float ratio = maxFrequency / minFrequency;
        float t = static_cast<float>(value) / 4095.0f;
        this->setFrequency(minFrequency * powf(ratio, t));
    }
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

void LFO::enableTempoSync()
{
    tempoSync = true;
    updateFrequencyFromTempoSync();
}

void LFO::disableTempoSync()
{
    tempoSync = false;
    this->setFrequency(frequency);
}

void LFO::setSyncBPM(float value)
{
    syncBpm = value;
    updateFrequencyFromTempoSync();
}

void LFO::setSyncRate(SyncRate rate)
{
    syncRate = rate;
    updateFrequencyFromTempoSync();
}

void LFO::updateFrequencyFromTempoSync()
{
    if (tempoSync) {
        const float beatsPerCycle = syncRateBeatsPerCycle();
        const float beatsPerSecond = syncBpm / 60.0f;
        const float syncedHz = beatsPerSecond / beatsPerCycle;
        setFrequency(syncedHz);
    }
}

float LFO::syncRateBeatsPerCycle() const
{
    switch (syncRate)
    {
    case SyncRate::WHOLE:             return 4.0f;        // 1/1
    case SyncRate::THREE_QUARTERS:    return 3.0f;        // 3/4
    case SyncRate::HALF:              return 2.0f;        // 1/2
    case SyncRate::QUARTER:           return 1.0f;        // 1/4
    case SyncRate::QUARTER_TRIPLET:   return 1.0f / 3.0f; // 1/4T
    case SyncRate::EIGHTH:            return 0.5f;        // 1/8
    case SyncRate::EIGHTH_TRIPLET:    return 1.0f / 3.0f; // 1/8T
    case SyncRate::SIXTEENTH:         return 0.25f;       // 1/16
    default:                          return 1.0f;
    }
}