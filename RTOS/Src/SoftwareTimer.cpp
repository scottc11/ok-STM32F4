#include "SoftwareTimer.h"

SoftwareTimer::SoftwareTimer() {}

SoftwareTimer::~SoftwareTimer()
{
    detachCallback();
    xTimerDelete(handle, 1);
}

/**
 * @brief Start the software timer
 * @note Do not use a block time if calling a timer API function from a timer callback function, as doing so could cause a deadlock!
 * @note  If the timer had already been started and was already in the active state, then xTimerStart()
 * has equivalent functionality to the xTimerReset() API function.
 */
void SoftwareTimer::start(TickType_t delay /*= 0*/)
{
    if (!xTimerIsTimerActive(handle))
    {
        xTimerStart(handle, delay);
    }
}

void SoftwareTimer::startFromISR(TickType_t delay /*= 0*/)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTimerStartFromISR(handle, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/**
 * @brief stops a timer that was previously started
 * @note Do not use a block time if calling a timer API function from a timer callback function, as doing so could cause a deadlock!
 */
void SoftwareTimer::stop(TickType_t delay /*= 0*/)
{
    xTimerStop(handle, delay);
}

/**
 * @brief stops a timer that was previously started
 * @note Do not use a block time if calling a timer API function from a timer callback function, as doing so could cause a deadlock!
 */
void SoftwareTimer::stopFromISR(TickType_t delay /*= 0*/)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTimerStopFromISR(handle, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken != pdFALSE)
    {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

/**
 * @brief re-starts a timer that was previously created using the xTimerCreate() API function.
 * @note If the timer had already been started and was already in the active state, then xTimerReset() will
 * cause the timer to re-evaluate its expiry time so that it is relative to when xTimerReset() was called.
 * @note If the timer was in the dormant state then xTimerReset() has equivalent functionality to the xTimerStart() API function.
 * @note Do not use a block time if calling a timer API function from a timer callback function, as doing so could cause a deadlock!
 */
void SoftwareTimer::reset(TickType_t delay /*= 0*/)
{
    xTimerReset(handle, delay);
}

/**
 * @brief re-starts a timer that was previously created using the xTimerCreate() API function.
 * @note Do not use a block time if calling a timer API function from a timer callback function, as doing so could cause a deadlock!
 */
void SoftwareTimer::resetFromISR(TickType_t delay /*= 0*/)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xTimerResetFromISR(handle, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken != pdFALSE)
    {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

/**
 * @brief Check if the timer is currently running
 * @return true if the timer is active, false if it is dormant
 */
bool SoftwareTimer::isRunning()
{
    return xTimerIsTimerActive(handle) == pdTRUE;
}

/**
 * @brief Get the period of the timer
 * @return the period of the timer in ticks
 */
TickType_t SoftwareTimer::getPeriod()
{
    return xTimerGetPeriod(handle);
}

/**
 * @brief change the period of an active or dormant state timer. Changing the period of a dormant timers will also start the timer.
 *
 * @param period The new period for timer instance. Timer periods are specified in tick periods or millisecond periods based on value of useMilliseconds param
 * @param useMilliseconds if true, period represents milliseconds, else period equals number of ticks
 *
 * @note the frequency of 1 'tick' is based on the value of FreeRTOS configTICK_RATE_HZ, which is usually set to 1000Hz
 */
void SoftwareTimer::setPeriod(TickType_t period, bool useMilliseconds /*false*/)
{
    if (useMilliseconds) {
        period = (period / portTICK_PERIOD_MS);
    }
    xTimerChangePeriod(handle, period, 100);
}

/**
 * @brief attach a callback for the timer to execute
 * 
 * @param func 
 * @param period frequency
 * @param repeated one-shot or autoreload
 */
void SoftwareTimer::attachCallback(Callback<void()> func, TickType_t period, bool repeated)
{
    _callback = func;
    handle = xTimerCreate(
        "timer",                     // name for timer
        period,                      // period of timer in ticks
        repeated ? pdTRUE : pdFALSE, // auto-reload flag
        this,                        // unique ID for timer
        callbackHandler);            // callback function
}

void SoftwareTimer::detachCallback()
{
    _callback = 0;
}

void SoftwareTimer::callbackHandler(TimerHandle_t xTimer)
{
    SoftwareTimer *timer = static_cast<SoftwareTimer *>(pvTimerGetTimerID(xTimer));
    if (timer->_callback)
    {
        timer->_callback();
    }   
}