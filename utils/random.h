#pragma once

#include "common.h"

void ok_random_seed(uint32_t seed);
uint32_t ok_random(int min, int max);
uint32_t ok_random_uint32();
uint16_t ok_random_uint16();
bool ok_random_coin_flip(uint32_t seed);
