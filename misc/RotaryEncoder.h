#pragma once

#include "common.h"
#include "DigitalIn.h"
#include "InterruptIn.h"

/**
 * @brief RotaryEncoder class. This class is used to detect the rotation of a rotary encoder.
 * @note Instead of using a timer, use a single interrupt pin to detect changes on one of the encoder pins,
 * and use a plain digital input on the other pin to detect the direction.
 * On pin A RISE, check if pin B is LOW, if so, it's a clockwise rotation, if it's HIGH, it's a counterclockwise rotation.
 */
class RotaryEncoder
{
public:
    RotaryEncoder(PinName _pinA, PinName _pinB, PinName _buttonPin);
    
    InterruptIn pinA;
    DigitalIn pinB;
    InterruptIn button;

    int position;
    uint8_t direction; // 1 for clockwise, 0 for counterclockwise
    bool pressed;

    Callback<void(uint8_t direction)> rotateCallback;
    Callback<void()> pressCallback;
    Callback<void()> releaseCallback;

    void onRotate();
    void onPress();
    void onRelease();

    void attachRotateCallback(Callback<void(uint8_t direction)> callback);
    void attachPressCallback(Callback<void()> callback);
    void attachReleaseCallback(Callback<void()> callback);
};