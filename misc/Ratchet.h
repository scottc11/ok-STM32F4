#pragma once

#include "main.h"
#include "Callback.h"

#ifndef PPQN
#define PPQN 96
#endif

#define RATCHET_DIV_1 PPQN / 1
#define RATCHET_DIV_2 PPQN / 2
#define RATCHET_DIV_3 PPQN / 3
#define RATCHET_DIV_4 PPQN / 4
#define RATCHET_DIV_6 PPQN / 6
#define RATCHET_DIV_8 PPQN / 8
#define RATCHET_DIV_12 PPQN / 12
#define RATCHET_DIV_16 PPQN / 16

class Ratchet
{
public:
    
    Ratchet() {}

    Callback<void(bool state)> ratchetCallback; // triggers when a ratchet event occurs (high or low)

    uint16_t input_idle;             // when input equals this, not ratcheting takes place
    uint16_t input_min;              // when input equals this, ratcheting is at max rate
    uint16_t input_max;              // when input equals this, ratcheting is at min rate
    uint16_t ratchetThresholds[8];
    uint8_t currRatchetRate;
    uint8_t prevRatchetRate;

    void init(uint16_t idle, uint16_t min, uint16_t max);
    void setRatchetThresholds(uint16_t idle, uint16_t min, uint16_t max);
    uint8_t calculateRatchet(uint16_t input);
    bool handleRatchet(int position, uint16_t input);

    void attachCallback(Callback<void(bool state)> func);
};