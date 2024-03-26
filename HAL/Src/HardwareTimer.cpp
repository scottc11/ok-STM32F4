#include "HardwareTimer.h"

HardwareTimer *HardwareTimer::TIM_INSTANCES[TIM_MAX_NUM_INSTANCES] = {0};

/**
 * @brief 
 * 
 * TIM1, TIM8: Advanced-Control, 16-bit
 * TIM2, TIM5: General Purpose, 32-bit
 * TIM3, 4, 9, 10, 11, 12, 13, 14: General Purpose 16-bit
 * TIM6, 7: Basic 16-bit
 * 
 * @param instance 
 * @param prescaler 
 * @param period 
 */
void HardwareTimer::init(uint32_t prescaler, uint32_t period)
{
    this->htim.Instance = this->_instance;
    this->htim.Init.Prescaler = prescaler;
    this->htim.Init.Period = period;
    this->htim.Init.CounterMode = TIM_COUNTERMODE_UP;
    this->htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    this->htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    tim_enable(this->_instance);  // start periph clocks etc.
    HAL_TIM_Base_Init(&htim);     // Configure the timer

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&htim, &sClockSourceConfig);
}

/**
 * @brief Start the timer (in interrupt mode)
 * 
 */
void HardwareTimer::start() {
    HAL_TIM_Base_Start_IT(&htim);
}

/**
 * @brief Stop the timer (in interrupt mode)
 * 
 */
void HardwareTimer::stop() {
    HAL_TIM_Base_Stop_IT(&htim);
}

/**
 * @brief Set the frequency (in HZ) the timer should overflow / trigger its interrupt
 * 
 * @param freq_hz ex. 1 == 1 second, 2, 0.5s, etc.
 */
void HardwareTimer::setOverflowFrequency(uint32_t freq_hz)
{
    tim_set_overflow_freq(&this->htim, freq_hz);
}

void HardwareTimer::attachOverflowCallback(Callback<void()> callback) {
    overflowCallback = callback;
}

void HardwareTimer::dettachOverflowCallback() {
    overflowCallback = NULL;
}

void HardwareTimer::handleOverflowInterrupt() {
    if (overflowCallback)
    {
        overflowCallback();
    }
}

/**
 * @brief static method which gets called when any TIM interrupt occurs, and routes them to the correct HardwareTimer instance.
 * 
 * @param _htim 
 */
void HardwareTimer::RoutePeriodElapsedCallback(TIM_HandleTypeDef *_htim)
{
    // if (this->htim->Instance == _htim->Instance) {
    //     this->handleOverflowInterrupt();
    // }
    for (auto ins : TIM_INSTANCES)
    {
        if (ins && ins->htim.Instance == _htim->Instance) // if instance not NULL
        {
            ins->handleOverflowInterrupt();
        }
    }
}

/**
 * @brief static method routes instance specific global timer interrupts into single function
 * 
 * @param instance ex. TIM6
 */
void HardwareTimer::RouteTimerGlobalInterrupt(TIM_TypeDef *instance) {
    for (auto ins : TIM_INSTANCES)
    {
        if (ins && ins->htim.Instance == instance) // if instance not NULL
        {
            HAL_TIM_IRQHandler(&ins->htim);
        }
    }
}