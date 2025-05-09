#include "Metronome.h"

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim4;

Metronome *Metronome::instance = NULL;

void Metronome::init()
{
    this->initTIM2(160, 0xFFFFFFFF - 1); // precaler value handles BPM range 40..240
    this->initTIM4(160, 10000 - 1);
}

void Metronome::start()
{
    this->reset();
    HAL_StatusTypeDef status;
    status = HAL_TIM_IC_Start_IT(&htim2, captureChannel);
    error_handler(status);
    status = HAL_TIM_Base_Start_IT(&htim4);
    error_handler(status);
}

void Metronome::stop() {
    HAL_TIM_IC_Stop_IT(&htim2, captureChannel);
    HAL_TIM_Base_Stop_IT(&htim4);
}

void Metronome::reset()
{
    __HAL_TIM_SetCounter(&htim2, 0); // not certain this has to happen, just assuming
    __HAL_TIM_SetCounter(&htim4, 0);
    this->pulse = 0;
    if (externalInputMode)
    {
        this->step = 99; // this is a hack so that handleStep in the input capture interrupt causes an overflow back to 0
    }
    else
    {
        this->step = 0;
    }
    if (resetCallback)
        resetCallback(pulse);
}

/**
 * @brief Set the BPM of the metronome
 * This internally calculates the ticks per beat based on the BPM and PPQN and 
 * sets the TIM4 period to the calculated value
 * @param bpm 
 */
void Metronome::setBPM(float bpm)
{
    
    uint32_t timerClockFrequency = tim_get_APBx_freq(&htim4);    // Get the timer clock bus frequency
    uint32_t prescaler = 100;                                    // note: a good prescaler value for a 16 bit timer @ 90MHz is 100, which allows for a BPM range of 40 to 240
    uint32_t counterFrequency = timerClockFrequency / (prescaler + 1); // Calculate the counter frequency

    // Calculate the ticks per beat
    uint32_t ticksPerBeat = counterFrequency / (bpm * PPQN / 60);

    // Configure the timer period
    __HAL_TIM_SET_PRESCALER(&htim4, prescaler);
    __HAL_TIM_SetAutoreload(&htim4, ticksPerBeat - 1); // Set the auto-reload register
}

void Metronome::setStepsPerBar(int steps)
{
    if (steps < 3 || steps > 7) return;
    stepsPerBar = steps;
}

uint8_t Metronome::getStepsPerBar() {
    return stepsPerBar;
}

void Metronome::handleStep()
{
    if (step < stepsPerBar - 1)
    {
        step++;
    }
    else
    {
        step = 0;
        if (barResetCallback)
            barResetCallback();
    }
}

/**
 * @brief initialize TIM2 as a slave to TIM1
 * @param prescaler setting to 1 should be best
 * @param period setting to 65535 should be best
 */
void Metronome::initTIM2(uint16_t prescaler, uint32_t period) // isn't TIM2 a 32-bit timer? This could greatly increase the resolution of frequency detection.
{
    __HAL_RCC_TIM2_CLK_ENABLE(); // turn on timer clock

    gpio_config_input_capture(capturePin, TIM_2);

    /* TIM2 interrupt Init */
    HAL_NVIC_SetPriority(TIM2_IRQn, RTOS_ISR_DEFAULT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_IC_InitTypeDef sConfigIC = {0};
    HAL_StatusTypeDef status;

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = prescaler;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = period;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    status = HAL_TIM_Base_Init(&htim2);
    if (status != HAL_OK)
        error_handler(status);

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig);

    status = HAL_TIM_IC_Init(&htim2);
    if (status != HAL_OK)
        error_handler(status);

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    status = HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);
    if (status != HAL_OK)
        error_handler(status);

    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1; // dedicated prescaler allows to "slow down" the frequency of the input signal
    sConfigIC.ICFilter = 0;                 // filter used to "debounce" the input signal
    status = HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, captureChannel);
    if (status != HAL_OK)
        error_handler(status);
}

/**
 * @brief initialize TIM4 as a master to TIM2
 * @param prescaler setting to 1 should be best
 * @param period setting to 65535 should be best
 */
void Metronome::initTIM4(uint16_t prescaler, uint16_t period)
{
    __HAL_RCC_TIM4_CLK_ENABLE();

    HAL_NVIC_SetPriority(TIM4_IRQn, RTOS_ISR_DEFAULT_PRIORITY, 0);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim4.Instance = TIM4;
    htim4.Init.Prescaler = prescaler;
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = period;
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Base_Init(&htim4);

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig);

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig);
}

/**
 * @brief Take a 16-bit ADC read and map it between the min ticksPerPulse and max ticksPerPulse
 * NOTE: because a lower values actually mean faster tempos, inverting the incoming value is needed
 * @param min the minimum ADC input value
 * @param max the maximum ADC input value
 * @param value this value gets inverted
 */
uint16_t Metronome::convertADCReadToTicks(uint16_t min, uint16_t max, uint16_t value)
{
    value = (max - value) + min; // invert
    uint16_t ticks = map_num_in_range<uint16_t>(value, min, max, MIN_TICKS_PER_PULSE, MAX_TICKS_PER_PULSE);
    return ticks;
}

/**
 * @brief Set the TIM4 overflow frequency
 *
 * @param ticks
 */
void Metronome::setPulseFrequency(uint32_t ticks)
{
    ticksPerPulse = ticks; // store for debugging reference
    __HAL_TIM_SetAutoreload(&htim4, ticks);
}

/**
 * The key in this callback is to reset the sequencer and/or clock step to 0 (ie. step 1)
 * This way, the sequence will always be in sync with the beat because the rising edge of the external tempo signal will always line up to tick 0 of the sequence.
 *
 * The caveat being we risk losing ticks near the end of each external clock signal, because the callback could execute before the final few ticks have a chance to be executed.
 * This could cause issues if there is important sequence data in those last remaining ticks, such as setting a trigger output high or low.
 *
 * To remedy this problem, we could keep track of how many ticks have executed in the sequence for each clock signal, so that if the HAL_TIM_IC_CaptureCallback executes prior to
 * the last tick in the sequence, we could quickly execute all those remaining ticks in rapid succession - which could sound weird, but likely neccessary.
 *
 * NOTE: The downfall of this approach is that some steps of the sequence will be missed when the external tempo signal increases,
 * but this should be a more musical way to keeping things in sync.
 *
 * NOTE: When jumping from a really fast tempo to a really slow tempo, the sequence steps will progress much faster than the incoming tempo (before the IC Calculation has time to update the sequencer pulse duration)
 * This means the sequence could technically get several beats ahead of any other gear.
 * To handle this, you could prevent the next sequence step from occuring if all sub-steps of the current step have been executed prior to a new IC event
 */
void Metronome::handleInputCaptureCallback()
{
    // almost always, there will need to be at least 1 pulse not yet executed prior to an input capture,
    // so you must execute all remaining until
    if (pulse < PPQN - 1)
    {
        handleStep();
        if (correctionCallback)
        {
            correctionCallback(pulse);
        }
    }

    // Get the current capture value
    uint32_t currentCapture = __HAL_TIM_GetCompare(&htim2, captureChannel);
    
    // Store the previous capture value for future use
    static uint32_t previousCapture = 0;
    
    // Calculate period between captures, handling potential overflow
    uint32_t inputCapture;
    if (previousCapture == 0) {
        // First valid capture after reset
        inputCapture = currentCapture;
    } else {
        // Calculate period between captures
        inputCapture = tim_get_capture_period(&htim2, currentCapture, previousCapture);
    }
    
    // Update previous capture for next calculation
    previousCapture = currentCapture;
    
    // Reset only the TIM4 counter, keep TIM2 running
    __HAL_TIM_SetCounter(&htim4, 0);
    __HAL_TIM_ENABLE(&htim4);     // re-enable TIM4 (it gets disabled should the pulse count overtake PPQN before a new input capture event occurs)
    
    this->setPulseFrequency(inputCapture / PPQN);
    this->pulse = 0;
    this->handleOverflowCallback();

    if (input_capture_callback)
        input_capture_callback();
}

void Metronome::enableInputCaptureISR()
{
    externalInputMode = true;
    // HAL_TIM_IC_Start_IT(&htim2, captureChannel);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

void Metronome::disableInputCaptureISR()
{
    externalInputMode = false;
    // HAL_TIM_IC_Stop(&htim2, captureChannel);
    __HAL_TIM_ENABLE(&htim4); // re-enable TIM4 (it gets disabled should the pulse count overtake PPQN before a new input capture event occurs)
    HAL_NVIC_DisableIRQ(TIM2_IRQn);
}

/**
 * @brief this callback gets called everytime TIM4 overflows.
 * Increments pulse counter
 */
void Metronome::handleOverflowCallback()
{
    if (ppqnCallback)
        ppqnCallback(pulse); // when clock inits, this ensures the 0ith pulse will get handled

    // by checking this first, you have the chance to reset any sequences prior to executing their 0ith pulse
    if (pulse == 0)
    {
        if (stepCallback)
            stepCallback(step);
    }

    if (pulse < PPQN - 1)
    {
        pulse++;
    }
    else
    {
        if (externalInputMode)
        {
            // external input will reset pulse to 0 and resume TIM4 in input capture callback
            __HAL_TIM_DISABLE(&htim4); // halt TIM4 until a new input capture event occurs
            handleStep();
        }
        else
        {
            pulse = 0;
            handleStep();
        }
    }
}

void Metronome::handleTransportInterruptPPQN()
{
    if (externalPulseMode)
    {
        this->handleOverflowCallback();
    }
}

void Metronome::enableExternalPulseMode(bool enable) {
    externalPulseMode = enable;
    if (enable == true) {
        HAL_TIM_IC_Stop_IT(&htim2, captureChannel);
        HAL_TIM_Base_Stop_IT(&htim4);
        extPulseInput.rise(callback(this, &Metronome::handleTransportInterruptPPQN));
        extResetInput.rise(callback(this, &Metronome::reset));
    }
}

void Metronome::attachInputCaptureCallback(Callback<void()> func)
{
    input_capture_callback = func;
}

void Metronome::attachPPQNCallback(Callback<void(uint8_t pulse)> func)
{
    ppqnCallback = func;
}

void Metronome::attachStepCallback(Callback<void(uint16_t step)> func)
{
    stepCallback = func;
}

void Metronome::attachResetCallback(Callback<void(uint8_t pulse)> func)
{
    resetCallback = func;
}

void Metronome::attachBarResetCallback(Callback<void()> func)
{
    barResetCallback = func;
}

void Metronome::attachCorrectionCallback(Callback<void(uint8_t pulse)> func)
{
    correctionCallback = func;
}

/**
 * @brief static function to handle timer period ellapsed interrupt
 * 
 * @param htim 
 */
void Metronome::RouteOverflowCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim4)
    {
        instance->handleOverflowCallback();
    }
}


/**
 * @brief static function to handle timer input capture interrupt
 * 
 * @param htim 
 */
void Metronome::RouteCaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        instance->handleInputCaptureCallback();
    }
}

/**
 * @brief static function to route timer global interrupt
 * 
 * @param tim_instance 
 */
void Metronome::RouteTimerGlobalInterrupt(TIM_TypeDef *tim_instance)
{
    if (instance) // only trigger if metronome being used
    {
        if (tim_instance == TIM2)
        {
            HAL_TIM_IRQHandler(&htim2);
        }
        else if (tim_instance == TIM4)
        {
            HAL_TIM_IRQHandler(&htim4);
        }
    }
}