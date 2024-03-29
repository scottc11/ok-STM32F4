#pragma once

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
    Callback<void()> overflowCallback;

    void init(uint32_t prescaler, uint32_t period);
    void start();
    void stop();
    // void reset();
    void setOverflowFrequency(uint32_t freq_hz);
    void handleOverflowCallback();

    void attachOverflowCallback(Callback<void()> callback);
    void detachOverflowCallback();

    static HardwareTimer *TIM_INSTANCES[TIM_MAX_NUM_INSTANCES];
    static void RoutePeriodElapsedCallback(TIM_HandleTypeDef *_htim);
    static void RouteTimerGlobalInterrupt(TIM_TypeDef *instance);
};