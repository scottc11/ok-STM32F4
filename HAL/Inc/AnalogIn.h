#pragma once

#include "main.h"
#include "tim_api.h"
#include "Callback.h"
#include "filters.h"
#include "okSemaphore.h"
#include "okQueue.h"
#include "uart_logger.h"

#define ADC_SAMPLE_COUNTER_LIMIT 2000
#define ADC_DEFAULT_INPUT_MAX BIT_MAX_16
#define ADC_DEFAULT_INPUT_MIN 0

#define ADC_MAX_INSTANCES 16

// if not defined, default to max number of ADC channels. This will take up more memory if not defined
#ifndef ADC_DMA_BUFF_SIZE
#define ADC_DMA_BUFF_SIZE ADC_MAX_INSTANCES
#endif

#ifndef ADC_SAMPLE_RATE_HZ
#define ADC_SAMPLE_RATE_HZ 2000
#endif

#ifndef ADC_TIM_PRESCALER
#define ADC_TIM_PRESCALER 100
#endif

#ifndef ADC_TIM_PERIOD
#define ADC_TIM_PERIOD 2000
#endif

/**
 * @brief Simple class that pulls the data from a DMA buffer into an object
 */
class AnalogIn
{
public:
    AnalogIn(PinName _pin);

    int index;         // represents position of sample data in DMA buffer array
    PinName pin;

    okSemaphore denoisingSemaphore;
    okSemaphore sampleSemaphore;
    uint16_t idleNoiseThreshold;               // how much noise an idle input signal contains
    uint16_t avgValueWhenIdle;                 // where the sensor sits when "idle" (only relevant for sensors)
    uint16_t noiseCeiling;                     // highest read noise value when idle
    uint16_t noiseFloor;                       // lowest read noise value when idle
    uint16_t inputMax = ADC_DEFAULT_INPUT_MAX; // highest read value from signal
    uint16_t inputMin = ADC_DEFAULT_INPUT_MIN; // lowest read value from signal

    Callback<void(uint16_t progress)> samplingProgressCallback;

    uint16_t read_u16();
    uint16_t read_u12();
    void setFilter(float value);
    void enableFilter() { filter = true; }
    void disableFilter() { filter = false; }
    void invertReadings();

    void log_noise_threshold_to_console(char const *source_id);
    void log_min_max(char const *source_id);

    okSemaphore *initDenoising();
    void sampleSignalNoise(uint16_t sample);

    okSemaphore *beginMinMaxSampling(uint16_t numSamples);
    void resetMinMax();
    void sampleMinMax(uint16_t sample);
    void setInputMax(uint16_t value);
    void setInputMin(uint16_t value);
    uint16_t getInputMax(void) { return inputMax; }
    uint16_t getInputMin(void) { return inputMin; }
    uint16_t getInputMedian(void) { return inputMin + ((inputMax - inputMin) / 2); }

    void sampleReadyCallback(uint16_t sample);
    void attachSamplingProgressCallback(Callback<void(uint16_t progress)> func);
    void detachSamplingProgressCallback();

    static void initialize(uint16_t sample_rate);
    static void setSampleRate(uint32_t sample_rate_hz);
    static void RouteConversionCompleteCallback();

    static uint16_t DMA_BUFFER[ADC_DMA_BUFF_SIZE];
    static AnalogIn *ADC_INSTANCES[ADC_DMA_BUFF_SIZE];
    static int NUM_ADC_INSTANCES; // how many AnalogIn instances are being insantiated (prior to )
    static SemaphoreHandle_t semaphore;

private:
    uint16_t currValue;
    uint16_t prevValue;
    bool filter = false;
    float filterAmount = 0.1;
    bool invert = false;

    uint16_t sampleTime;    // how long / how many samples you wish to sample for
    uint16_t sampleCounter; // basic counter for DSP
    bool samplingNoise;     // flag to tell handle whether to use the incloming data in the DMA_BUFFER for denoising an idle input signal
    bool samplingMinMax;
};