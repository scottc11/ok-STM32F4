/**
In the STM32 microcontrollers with the ADC oversampling feature, the ADC hardware
oversampling feature can be used for averaging. This feature simply performs the sum of a
given number of ADC raw samples into one final sample. This final sample can then be right
shifted to reduce the bit width caused by multiple ADC samples accumulation. All these
operations (accumulation and right-bit shifting) are performed by hardware. The ADC
hardware oversampling feature can be configured to process up to 1024 input samples
(depending on devices).
*/

#include "AnalogIn.h"

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
TIM_HandleTypeDef htim3;

SemaphoreHandle_t AnalogIn::semaphore;
AnalogIn *AnalogIn::ADC_INSTANCES[ADC_DMA_BUFF_SIZE] = {0};
uint16_t AnalogIn::DMA_BUFFER[ADC_DMA_BUFF_SIZE] = {0};
int AnalogIn::num_adc_instances = 0;

AnalogIn::AnalogIn(PinName _pin)
{
    pin = _pin;
    num_adc_instances++;
    // Add constructed instance to the static list of instances (required for IRQ routing)
    for (int i = 0; i < ADC_DMA_BUFF_SIZE; i++)
    {
        if (ADC_INSTANCES[i] == NULL)
        {
            index = i;
            ADC_INSTANCES[i] = this;
            break;
        }
    }
}

uint16_t AnalogIn::read_u16()
{
    return invert ? BIT_MAX_16 - currValue : currValue;
}

void AnalogIn::invertReadings()
{
    this->invert = !this->invert;
}

void AnalogIn::setFilter(float value)
{
    if (value == 0)
    {
        filter = false;
    }
    else if (value > (float)1)
    {
        // raise error
        filter = false;
    }
    else
    {
        prevValue = convert12to16(DMA_BUFFER[index]);
        filterAmount = value;
        filter = true;
    }
}

/**
 * @brief takes the denoising semaphore and gives it once calculation is finished.
 * NOTE: don't forget to "give()" the semaphore back after waiting for it.
 *
 * @return okSemaphore*
 */
okSemaphore *AnalogIn::initDenoising()
{
    this->samplingNoise = true;
    denoisingSemaphore.take(); // create a semaphore
    return &denoisingSemaphore;
}

// set this as a task so that in the main loop you block with a semaphore until this task gives the semaphore back (once it has completed)
void AnalogIn::sampleSignalNoise(uint16_t sample)
{
    // get max read, get min read, get avg read
    if (sampleCounter < ADC_SAMPLE_COUNTER_LIMIT)
    {
        if (sampleCounter == 0)
        {
            noiseCeiling = sample;
            noiseFloor = sample;
        }
        else
        {
            if (sample > noiseCeiling)
            {
                noiseCeiling = sample;
            }
            else if (sample < noiseFloor)
            {
                noiseFloor = sample;
            }
        }
        sampleCounter++;
    }
    else
    {
        samplingNoise = false;
        sampleCounter = 0; // reset back to 0 for later use
        idleNoiseThreshold = (noiseCeiling - noiseFloor) / 2;
        avgValueWhenIdle = noiseFloor + idleNoiseThreshold;
        denoisingSemaphore.give();
    }
}

/**
 * @brief This is not exactly a callback, its really a "sender" task
 *
 * @param sample
 */
void AnalogIn::sampleReadyCallback(uint16_t sample)
{
    prevValue = currValue;
    currValue = convert12to16(sample);
    if (filter)
    {
        currValue = filter_one_pole<uint16_t>(currValue, prevValue, filterAmount);
    }
    if (samplingNoise)
    {
        this->sampleSignalNoise(currValue);
        if (samplingProgressCallback)
            samplingProgressCallback(sampleCounter);
    }
    if (this->samplingMinMax) // execute if task has a semaphore?
    {
        if (sampleCounter < sampleTime)
        {
            this->sampleMinMax(currValue);
            sampleCounter++;
        }
        else
        {
            samplingMinMax = false;
            sampleCounter = 0;
            // maybe calulate median here
            sampleSemaphore.give();
        }
    }
}

/**
 * @brief A static member function which gets called as an ISR whenever the ADC DMA Conversion is completed
 *
 */
void AnalogIn::RouteConversionCompleteCallback() // static
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(AnalogIn::semaphore, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief This task waits for a semaphore to be made available by the RouteConversionCompleteCallback()
 * When it takes the semaphore, the task will trigger a callback function for every AnalogIn
 * instance that exists.
 *
 * @param params
 */
void AnalogIn::sampleReadyTask(void *params)
{
    logger_log_task_watermark();
    while (1)
    {
        xSemaphoreTake(AnalogIn::semaphore, portMAX_DELAY);
        for (auto ins : ADC_INSTANCES)
        {
            if (ins) // if instance not NULL
            {
                ins->sampleReadyCallback(AnalogIn::DMA_BUFFER[ins->index]);
            }
        }
    }
}

void AnalogIn::log_noise_threshold_to_console(char const *source_id)
{
    logger_log("\n");
    logger_log(source_id);
    logger_log(" ADC Noise: ");
    logger_log(this->idleNoiseThreshold);
    logger_log(", Idle: ");
    logger_log(this->avgValueWhenIdle);
}

void AnalogIn::log_min_max(char const *source_id)
{
    logger_log(source_id);
    logger_log(" Signal Max: ");
    logger_log(this->inputMax);
    logger_log(" || Signal Min: ");
    logger_log(this->inputMin);
    logger_log("\n");
}

/**
 * @brief resets the min and max input values to the currently read value on the ADC Pin
 */
void AnalogIn::resetMinMax()
{
    this->setInputMax(currValue + 1);
    this->setInputMin(currValue - 1);
}

void AnalogIn::setInputMax(uint16_t value)
{
    this->inputMax = value;
}

void AnalogIn::setInputMin(uint16_t value)
{
    this->inputMin = value;
}

/**
 * @brief
 *
 * @param numSamples how many samples ie. how long to sample for.
 * @return okSemaphore*
 */
okSemaphore *AnalogIn::beginMinMaxSampling(uint16_t numSamples)
{
    this->resetMinMax();
    this->sampleCounter = 0;
    this->sampleTime = numSamples;
    this->samplingMinMax = true;
    this->sampleSemaphore.take();
    return &sampleSemaphore;
}

/**
 * @brief determine a signals min and max values
 */
void AnalogIn::sampleMinMax(uint16_t sample)
{
    if (sample > inputMax)
    {
        this->setInputMax(sample);
    }
    else if (sample < inputMin)
    {
        this->setInputMin(sample);
    }
}

void AnalogIn::attachSamplingProgressCallback(Callback<void(uint16_t progress)> func)
{
    samplingProgressCallback = func;
}

void AnalogIn::detachSamplingProgressCallback()
{
    samplingProgressCallback = NULL;
}

/**
 * @brief Initializes ADC peripheral and creates a FreeRTOS task for handling incoming DMA ADC reads
 * @note Must be called in main() in order for ADC to work.
*/
void AnalogIn::initialize()
{
    // initialize DMA
    __HAL_RCC_DMA2_CLK_ENABLE(); /* DMA controller clock enable */
    
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, RTOS_ISR_DEFAULT_PRIORITY, 0); /* DMA2_Stream0_IRQn interrupt configuration */
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn); /* DMA interrupt init */

    
    // initalize ADC1 and pins
    for (int i = 0; i < num_adc_instances; i++) // note: using static member prevents initializing unwanted pins
    {
        if (ADC_INSTANCES[i] != NULL) {
            enable_adc_pin(ADC_INSTANCES[i]->pin);
        }
    }

    /* ADC1 DMA Init */
    hdma_adc1.Instance = DMA2_Stream0;
    hdma_adc1.Init.Channel = DMA_CHANNEL_0;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc1.Init.Mode = DMA_CIRCULAR;
    hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
    hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    HAL_DMA_Init(&hdma_adc1);

    __HAL_LINKDMA(&hadc1, DMA_Handle, hdma_adc1);

    // Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
    // NOTE: Important to set EOCSelection to "EOC Flag at the end of all converions" if you want to continuously read the ADC, otherwise the ADC will only do a single conversion
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = ENABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = ADC_DMA_BUFF_SIZE; // suspicious... should this be ADC_DMA_BUFF_SIZE?
    hadc1.Init.DMAContinuousRequests = ENABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
    HAL_ADC_Init(&hadc1);

    // Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
    ADC_ChannelConfTypeDef sConfig = {0};
    for (int i = 0; i < num_adc_instances; i++)
    {
        // get alternate function
        uint32_t channel;
        for (int y = 0; y < ADC_MAX_INSTANCES; y++)
        {
            if (ADC_INSTANCES[i]->pin == ADC_PIN_MAP[y].pin)
            {
                channel = ADC_PIN_MAP[y].channel;
                break;
            }
        }
        
        sConfig.Channel = channel;
        sConfig.Rank = ADC_INSTANCES[i]->index + 1;
        sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
        HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    }

    // initialize TIM3
    __HAL_RCC_TIM3_CLK_ENABLE();

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    htim3.Instance = TIM3;
    htim3.Init.Prescaler = ADC_TIM_PRESCALER;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = ADC_TIM_PERIOD;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Base_Init(&htim3);

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig);

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig);

    // set sample rate
    setSampleRate(ADC_SAMPLE_RATE_HZ);

    // create semaphore
    AnalogIn::semaphore = xSemaphoreCreateBinary();

    // create sample ready task
    xTaskCreate(AnalogIn::sampleReadyTask, "ADC Task", RTOS_STACK_SIZE_MIN, NULL, RTOS_PRIORITY_MED, NULL);

    // start timer
    HAL_TIM_Base_Start(&htim3);
    
    // start ADC in DMA mode
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)AnalogIn::DMA_BUFFER, ADC_DMA_BUFF_SIZE);
}

void AnalogIn::setSampleRate(uint32_t sample_rate_hz)
{
    switch (hadc1.Init.ClockPrescaler)
    {
    case ADC_CLOCK_SYNC_PCLK_DIV2:
        tim_set_overflow_freq(&htim3, sample_rate_hz * 2);
        break;
    case ADC_CLOCK_SYNC_PCLK_DIV4:
        tim_set_overflow_freq(&htim3, sample_rate_hz * 4);
        break;
    case ADC_CLOCK_SYNC_PCLK_DIV6:
        tim_set_overflow_freq(&htim3, sample_rate_hz * 6);
        break;
    case ADC_CLOCK_SYNC_PCLK_DIV8:
        tim_set_overflow_freq(&htim3, sample_rate_hz * 8);
        break;
    default:
        tim_set_overflow_freq(&htim3, sample_rate_hz * 2);
        break;
    }
}

/**
 * @brief This function handles DMA2 stream0 global interrupt.
 * NOTE: This must be declared when using an interupt peripheral.
 * When there is an interrupt for which no handler exists, "Default_Handler" will be called and put the system in an inifinite loop
 */
extern "C" void DMA2_Stream0_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_adc1);
}

/**
 * @brief  Regular conversion complete callback in non blocking mode
 * @param  hadc pointer to a ADC_HandleTypeDef structure that contains
 *         the configuration information for the specified ADC.
 * @retval None
 */
extern "C" void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    if (hadc->Instance == ADC1)
    {
        AnalogIn::RouteConversionCompleteCallback();
    }
}