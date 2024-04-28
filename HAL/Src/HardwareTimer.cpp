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
 * @brief Initialize a timer in input capture mode
 * 
 * @param instance 
 * @param pin gpio pin to use for input capture
 * @param _channel ex. TIM_CHANNEL_2
 * @param prescaler 
 * @param period 
 */
void HardwareTimer::initInputCapture(PinName pin, uint32_t _channel, uint16_t prescaler, uint32_t period)
{
    this->isInputCapture = true;
    this->channel = _channel;
    tim_enable(this->_instance); // init clock and interrupt

    intptr_t ptrInt = reinterpret_cast<intptr_t>(this->_instance);
    gpio_config_input_capture(pin, (TIMName)ptrInt); // GPIO Init

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_IC_InitTypeDef sConfigIC = {0};

    this->htim.Instance = this->_instance;
    this->htim.Init.Prescaler = prescaler;
    this->htim.Init.CounterMode = TIM_COUNTERMODE_UP;
    this->htim.Init.Period = period;
    this->htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    this->htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    HAL_TIM_Base_Init(&this->htim);

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&this->htim, &sClockSourceConfig);

    HAL_TIM_IC_Init(&this->htim);

    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigIC.ICFilter = 0;

    HAL_TIM_IC_ConfigChannel(&this->htim, &sConfigIC, channel);
}

/**
 * @brief Start the timer (in interrupt mode)
 * 
 */
void HardwareTimer::start() {
    isRunning = true;
    if (isInputCapture) {
        HAL_TIM_IC_Start_IT(&this->htim, channel);
    } else {
        HAL_TIM_Base_Start_IT(&htim);
    }
}

/**
 * @brief Stop the timer (in interrupt mode)
 * 
 */
void HardwareTimer::stop() {
    isRunning = false;
    if (isInputCapture) {
        HAL_TIM_IC_Stop_IT(&this->htim, channel);
    } else {
        HAL_TIM_Base_Stop_IT(&htim);
    }
}

void HardwareTimer::reset()
{
    __HAL_TIM_SetCounter(&this->htim, 0); // reset after each input capture
}

uint32_t HardwareTimer::getCompare()
{
    return __HAL_TIM_GetCompare(&this->htim, this->channel);
}

float HardwareTimer::calculateCaptureFrequency()
{
    uint32_t inputCapture = this->getCompare();
    uint16_t prescaler = this->htim.Init.Prescaler;
    uint32_t timerClockHz = tim_get_APBx_freq(&this->htim);
    return static_cast<float>(timerClockHz) / (inputCapture * (prescaler + 1));
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

void HardwareTimer::detachOverflowCallback() {
    overflowCallback = NULL;
}

void HardwareTimer::attachCaptureCallback(Callback<void()> callback) {
    captureCallback = callback;
}

void HardwareTimer::detachCaptureCallback() {
    captureCallback = NULL;
}

/**
 * @brief static method which gets called when any TIM interrupt occurs, and routes them to the correct HardwareTimer instance.
 * 
 * @param _htim 
 */
void HardwareTimer::RoutePeriodElapsedCallback(TIM_HandleTypeDef *_htim)
{
    for (auto ins : TIM_INSTANCES)
    {
        if (ins && ins->htim.Instance == _htim->Instance) // if instance not NULL
        {
            if (ins->overflowCallback)
            {
                ins->overflowCallback();
            }
            break;
        }
    }
}

void HardwareTimer::RouteCaptureCallback(TIM_HandleTypeDef *_htim)
{
    for (auto ins : TIM_INSTANCES)
    {
        if (ins && ins->htim.Instance == _htim->Instance) // if instance not NULL
        {
            if (ins->resetAfterCapture) // usually this is all you want to do after a capture
            {
                ins->reset();
            }
            if (ins->captureCallback)
            {
                ins->captureCallback();
            }
            break;
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
            break;
        }
    }
}