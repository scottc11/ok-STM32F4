#include "random.h"

// Constants for the LCG
#define LCG_A 1664525
#define LCG_C 1013904223
#define LCG_M 0xFFFFFFFF // 2^32

// Seed value
static uint32_t lcg_seed = 1;

/**
 * @brief seed the random number generator
 *
 * @note Initialize the seed once at the start of your application or when you need a new sequence of random numbers. This can be done using a varying source, such as a timer or an uninitialized memory location, to ensure different sequences across program runs.
 *
 * @param seed
 */
void ok_random_seed(uint32_t seed)
{
    lcg_seed = seed;
}

/**
 * @brief generate a random number between min and max
 *
 * @param min
 * @param max
 * @return uint32_t
 */
uint32_t ok_random(int min, int max)
{
    return ok_random_uint32() % (max - min + 1) + min;
}

/**
 * @brief generate a random number
 * @return uint32_t
 */
uint32_t ok_random_uint32()
{
    lcg_seed = (LCG_A * lcg_seed + LCG_C) & LCG_M;
    return lcg_seed;
}

/**
 * @brief generate a random uint16_t value
 * @return uint16_t
 */
uint16_t ok_random_uint16()
{
    uint32_t r = ok_random_uint32();
    return static_cast<uint16_t>(r >> 16); // use high bits (better quality)
    // or: return static_cast<uint16_t>((r >> 16) ^ r); // mixed
}

/**
 * @brief generate a random boolean value
 * @param seed you will want to pass a seed for a better coin flip
 * @return bool
 */
bool ok_random_coin_flip(uint32_t seed)
{
    ok_random_seed(seed);
    return ok_random_uint32() % 2 == 0;
}

/**
 * @brief Return a boolean, with probability of success (true) given by the probability parameter.
 * Other know as a "Bernoulli trial". A Bernoulli trial is a random experiment with two possible outcomes: success or failure.
 * The probability of success is given by the probability parameter.
 * The probability of failure is 1 - probability.
 * The result of the Bernoulli trial is a boolean value.
 *
 * @param probability 0..4095
 * @return true if success, false if failure
 */
uint8_t ok_random_bernoulli_gate(uint16_t probability) {
    if (probability == 0u)
    {
        return 0u;
    }

    if (probability >= 4095u)
    {
        return 1u;
    }

    // Compare in integer domain to avoid runtime division:
    // random/65535 <= probability/4095
    uint32_t lhs = static_cast<uint32_t>(ok_random_uint16()) * 4095u;
    uint32_t rhs = static_cast<uint32_t>(probability) * 65535u;
    return (lhs <= rhs) ? 1u : 0u;
}