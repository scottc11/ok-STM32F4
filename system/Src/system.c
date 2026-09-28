#include "system.h"

/**
 * @brief Initialize generic system peripherals
 * 
 * @return void
 * @note This function will set the brownout reset level to 2.70 to 3.60 V before releasing reset
 */
void system_init(void)
{
    // BOR level 3 needs to be set before the clock is switched to 180 MHz.
    // That holds the MCU in reset until VDD is in the 2.7–3.6 V range (which is required for overdrive).
    system_set_brownout_reset_level(OB_BOR_LEVEL3);
    SystemClock_Config();
}

/**
 * @brief Set the brownout reset level
 * 
 * @param level The brownout reset level to set (OB_BOR_LEVEL3, OB_BOR_LEVEL2, OB_BOR_LEVEL1, OB_BOR_OFF)
 * @return void
 * @note This function will reset the system if the brownout reset level is changed successfully
 */
void system_set_brownout_reset_level(uint8_t level)
{
    FLASH_OBProgramInitTypeDef obConfig = {0};

    HAL_FLASHEx_OBGetConfig(&obConfig);
    if (obConfig.BORLevel == level)
    {
        return;
    }

    if (HAL_FLASH_OB_Unlock() != HAL_OK)
    {
        return;
    }

    obConfig.BORLevel = level;

    if (HAL_FLASHEx_OBProgram(&obConfig) == HAL_OK && HAL_FLASH_OB_Launch() == HAL_OK)
    {
        HAL_NVIC_SystemReset();
    }
}