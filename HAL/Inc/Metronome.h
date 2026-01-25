#pragma once

#include "main.h"
#include "tim_api.h"
#include "Callback.h"
#include "Algorithms.h"
#include "InterruptIn.h"

#ifndef PPQN
#define PPQN 96
#endif

#define PPQN_8th (PPQN / 2)
#define PPQN_16th (PPQN / 4)

#define MAX_TICKS_PER_PULSE 34299 // (40 BPM)  MAX TIM4 tickers per pulse
#define MIN_TICKS_PER_PULSE 5716  // (240 BPM) MIN TIM4 tickers per pulse

enum class InputNoteDivision : uint32_t {
    QUARTER_NOTE = TIM_ICPSC_DIV1,    // capture every edge
    EIGHTH_NOTE = TIM_ICPSC_DIV2,     // capture every 2nd edge  
    SIXTEENTH_NOTE = TIM_ICPSC_DIV4, // capture every 4th edge
    THIRTY_SECOND_NOTE = TIM_ICPSC_DIV8 // capture every 8th edge
};

extern TIM_HandleTypeDef htim2; // 32-bit timer
extern TIM_HandleTypeDef htim4; // 16-bit timer

class Metronome
{
public:
    int pulse;              // the current PPQN
    uint8_t step;           // current step. Will never exceed value of stepsPerBar
    uint16_t ticksPerStep;  // how many TIM2 ticks per one step / quarter note
    uint16_t ticksPerPulse; // how many TIM2 ticks for one PPQN
    uint8_t stepsPerBar;    // value represents the number of quarter notes per bar (ie. 3/4, 4/4, 5/4, 6/4, 7/4)
    bool externalInputMode;
    bool externalPulseMode;  // when true, the metronome will use an external input signal to advance the clock by 1 PPQN
    bool bpmExceeded;        //
    InputNoteDivision inputNoteDivision;
    bool running;

    PinName capturePin;      // the pin to use for input capture (ex. PA_3)
    int captureChannel;      // the timer channel to use for input capture (ex. TIM_CHANNEL_2)
    uint32_t capturePeriod;  // the period of the last input capture

    InterruptIn extPulseInput;
    InterruptIn extResetInput;

    Callback<void()> tickCallback;              // this callback gets executed at a frequency equal to tim1_freq
    Callback<void()> barResetCallback;          // executes when clock.step exceeds the set time signature (ie. one bar)
    Callback<void()> input_capture_callback;    // this callback gets executed on the rising edge of an external input signal
    Callback<void(uint8_t pulse)> ppqnCallback; // this callback gets executed at a rate equal to input capture / PPQN. It passes the current tick values as arguments
    Callback<void(uint16_t step)> stepCallback; // executes every step
    Callback<void(uint8_t pulse)> resetCallback;
    Callback<void(uint8_t pulse)> correctionCallback; // executes when IC happens prior to PPQN overflow
    Callback<void()> overflowCallback; // callback executes when a full step completes
    Callback<void(uint32_t ticksPerPulse)> bpmExceededCallback; // callback executes when the external BPM exceeds the internal BPM limit
    Callback<void()> bpmStabilizedCallback; // callback executes when the external BPM returns to within the internal BPM limit

    /**
     * @brief Construct a new Metronome object
     * @param _capturePin the pin to use for input capture (ex. PA_3)
     * @param _captureChannel the timer channel to use for input capture (ex. TIM_CHANNEL_2)
     * @param pulse_pin the pin to use for transport pulse input
     * @param reset_pin the pin to use for transport reset input
     */
    Metronome(PinName _capturePin, int _captureChannel, PinName pulse_pin = NC, PinName reset_pin = NC) : extPulseInput(pulse_pin), extResetInput(reset_pin)
    {
        instance = this;
        capturePin = _capturePin;
        captureChannel = _captureChannel;
        ticksPerStep = 11129;
        ticksPerPulse = ticksPerStep / PPQN;
        stepsPerBar = 4;
        inputNoteDivision = InputNoteDivision::QUARTER_NOTE;
        running = false;
    };

    void init();
    void initTIM2(uint16_t prescaler, uint32_t period);
    void initTIM4(uint16_t prescaler, uint16_t period);

    void start();
    void stop();
    void reset();
    void setBPM(float bpm);
    void setInputNoteDivision(InputNoteDivision division);
    void setStepsPerBar(int steps);
    uint8_t getStepsPerBar();
    void handleStep();

    void setPulseFrequency(uint32_t ticks);
    uint16_t convertADCReadToTicks(uint16_t min, uint16_t max, uint16_t value);
    void enableInputCaptureISR();
    void disableInputCaptureISR();
    void enableExternalPulseMode(bool enable);

    // Callback Setters
    void attachInputCaptureCallback(Callback<void()> func);
    void attachPPQNCallback(Callback<void(uint8_t pulse)> func);
    void attachStepCallback(Callback<void(uint16_t step)> func);
    void attachResetCallback(Callback<void(uint8_t pulse)> func);
    void attachBarResetCallback(Callback<void()> func);
    void attachCorrectionCallback(Callback<void(uint8_t pulse)> func);
    void attachBPMExceededCallback(Callback<void(uint32_t capture)> func);
    void attachBPMStabilizedCallback(Callback<void()> func);

    // Low Level HAL interupt handlers
    void handleInputCaptureCallback();
    void handleOverflowCallback();
    void handleTransportInterruptPPQN();
    static void RouteOverflowCallback(TIM_HandleTypeDef *htim);
    static void RouteCaptureCallback(TIM_HandleTypeDef *htim);
    static void RouteTimerGlobalInterrupt(TIM_TypeDef *tim_instance);

private:
    static Metronome *instance;
};