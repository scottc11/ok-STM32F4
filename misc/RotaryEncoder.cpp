#include "RotaryEncoder.h"

RotaryEncoder::RotaryEncoder(PinName _pinA, PinName _pinB, PinName _buttonPin)
    : pinA(_pinA, PullUp), pinB(_pinB, PullUp), button(_buttonPin, PullUp)
{
    pinA.fall(callback(this, &RotaryEncoder::onRotate));
    button.fall(callback(this, &RotaryEncoder::onPress));
    button.rise(callback(this, &RotaryEncoder::onRelease));
}

void RotaryEncoder::onRotate()
{
    if (pinB.read() == HIGH) // Clockwise rotation
    {
        direction = 1;
        position++;
    }
    else // Counterclockwise rotation
    { 
        direction = 0;
        position--;
    }

    if (rotateCallback)
        rotateCallback(direction);
}

void RotaryEncoder::onPress()
{
    if (pressCallback)
        pressCallback();
}

void RotaryEncoder::onRelease()
{
    if (releaseCallback)
        releaseCallback();
}
/**
 * @brief Attach a callback to the rotate event. This callback is called when the rotary encoder is rotated.
 * @param callback The callback to attach
 * @note The callback is called with the direction of the rotation. 1 for clockwise, 0 for counterclockwise.
 */
void RotaryEncoder::attachRotateCallback(Callback<void(uint8_t direction)> callback)
{
    rotateCallback = callback;
}

/**
 * @brief Attach a callback to the press event. This callback is called when the rotary encoder button is pressed.
 * @param callback The callback to attach
 */
void RotaryEncoder::attachPressCallback(Callback<void()> callback)
{
    pressCallback = callback;
}

/**
 * @brief Attach a callback to the release event. This callback is called when the rotary encoder button is released.
 * @param callback The callback to attach
 */
void RotaryEncoder::attachReleaseCallback(Callback<void()> callback)
{
    releaseCallback = callback;
}