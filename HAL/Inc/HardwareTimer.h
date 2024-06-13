#pragma once

#include <math.h>
#include "common.h"
#include "Callback.h"
#include "tim_api.h"

#define TIM_MAX_NUM_INSTANCES 14

class HardwareTimer {
public:
    
    HardwareTimer(TIM_TypeDef *instance) {
        _instance = instance;
        // Add constructed instance to the static list of instances (required for IRQ routing)
        for (int i = 0; i < TIM_MAX_NUM_INSTANCES; i++)
        {
            if (TIM_INSTANCES[i] == NULL)
            {
                TIM_INSTANCES[i] = this;
                break;
            }
        }
    }

    TIM_TypeDef *_instance;
    TIM_HandleTypeDef htim;
    uint32_t channel;               // the timer channel to use for various timer functions
    
    uint32_t overflowCount = 0;
    uint32_t prevOverflowCount = 0;
    int prevCapture = 0;
    int currCapture = 0;
    float captureFrequency = 0.0;
    
    bool isInputCapture = false;
    bool isRunning = false;

    Callback<void()> overflowCallback;
    Callback<void()> captureCallback;

    void init(uint32_t prescaler, uint32_t period);
    void initInputCapture(PinName pin, uint32_t _channel, uint16_t prescaler, uint32_t period);
    void start();
    void stop();
    void reset();
    uint32_t getCapture();
    float calculateCaptureFrequency();
    void setOverflowFrequency(uint32_t freq_hz);
    void setCapturePrescaler(uint16_t prescaler);
    uint8_t getCapturePrescaler();

    void attachOverflowCallback(Callback<void()> callback);
    void detachOverflowCallback();
    void attachCaptureCallback(Callback<void()> callback);
    void detachCaptureCallback();

    static HardwareTimer *TIM_INSTANCES[TIM_MAX_NUM_INSTANCES];
    static void RoutePeriodElapsedCallback(TIM_HandleTypeDef *_htim);
    static void RouteCaptureCallback(TIM_HandleTypeDef *_htim);
    static void RouteTimerGlobalInterrupt(TIM_TypeDef *instance);
};