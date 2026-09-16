#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Jump to the STM32 system bootloader if the last reset was a USB DFU
 * detach request, otherwise return immediately.
 *
 * Must be the first statement in main(), before HAL_Init() and before any
 * peripheral is configured. Does not return when it takes the jump.
 */
void usb_device_check_bootloader_entry(void);

/**
 * @brief Bring up OTG_FS, start the TinyUSB device stack, and spawn the task
 * that services it.
 *
 * Must be called after SystemClock_Config() and before the scheduler starts.
 */
void usb_device_init(void);

#ifdef __cplusplus
}
#endif
