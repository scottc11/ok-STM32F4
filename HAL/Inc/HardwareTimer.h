#pragma once

#include "common.h"
#include "Callback.h"
#include "tim_api.h"
#include "gpio_api.h"

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
    
    uint32_t overflowCount = 0;
    uint32_t prevOverflowCount = 0;
    
    bool running = false;

    Callback<void()> overflowCallback;

    void init(uint32_t prescaler, uint32_t period);
    
    void initInputCapture(uint16_t prescaler, uint32_t period);
    void configureCaptureChannel(PinName pin, uint32_t _channel);
    void startCaptureChannel(uint32_t _channel);
    void stopCaptureChannel(uint32_t _channel);
    uint32_t getCapture(uint32_t _channel);

    void start();
    void stop();
    void reset();

    void setOverflowFrequency(uint32_t freq_hz, uint32_t maxTimerResolution = 65535);
    void setPrescaler(uint16_t prescaler);
    void setPeriod(uint32_t period);

    void setCapturePrescaler(uint32_t channel, uint16_t prescaler);
    uint8_t getCapturePrescaler(uint32_t channel);

    void attachOverflowCallback(Callback<void()> callback);
    void detachOverflowCallback();

    static HardwareTimer *TIM_INSTANCES[TIM_MAX_NUM_INSTANCES];
    static void RoutePeriodElapsedCallback(TIM_HandleTypeDef *_htim);
    static void RouteTimerGlobalInterrupt(TIM_TypeDef *instance);
};