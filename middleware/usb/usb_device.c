#include "usb_device.h"

#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx_hal.h"
#include "tusb.h"

#define USB_TASK_STACK_SIZE 512
#define USB_TASK_PRIORITY   4

// Base of the ST factory bootloader in system memory (AN2606, STM32F446).
#define SYSTEM_MEMORY_BASE 0x1FFF0000u

#define BOOTLOADER_MAGIC 0xB00710ADu

static void usb_device_task(void *params);

// Survives the warm reset that carries us into the bootloader. Lives in the
// .noinit section defined by STM32F446RETx_FLASH.ld so startup does not zero it.
static uint32_t bootloader_magic __attribute__((section(".noinit")));

/**
 * @brief Route the 48 MHz USB clock through PLLSAI-P.
 *
 * The main PLL on this board is tuned for a 180 MHz SYSCLK, which leaves its Q
 * output unable to produce exactly 48 MHz, so OTG_FS is fed from PLLSAI
 * instead.
 */
static void usb_clock_init(void)
{
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_CLK48;
    PeriphClkInitStruct.PLLSAI.PLLSAIM = 4;
    PeriphClkInitStruct.PLLSAI.PLLSAIN = 96;
    PeriphClkInitStruct.PLLSAI.PLLSAIQ = 2;
    PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV4;
    PeriphClkInitStruct.PLLSAIDivQ = 1;
    PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48CLKSOURCE_PLLSAIP;

    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
}

static void usb_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();

    // PA11 -> USB_OTG_FS_DM, PA12 -> USB_OTG_FS_DP.
    // PA9 (VBUS) and PA10 (ID) are unconnected on this board.
    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    __HAL_RCC_USB_OTG_FS_CLK_ENABLE();
}

void usb_device_check_bootloader_entry(void)
{
    if (bootloader_magic != BOOTLOADER_MAGIC)
    {
        return;
    }

    // Clear before jumping: a bootloader that fails to start must not leave the
    // board stuck in a reset loop.
    bootloader_magic = 0;

    // Called from the top of main(), so nothing needs tearing down first: no HAL,
    // no peripherals, no scheduler, and every global in this project only
    // registers itself in its constructor rather than touching hardware. All that
    // remains is to map system memory to address 0, which is where the bootloader
    // expects its own vector table.
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();

    // Read both vectors straight from absolute addresses: after __set_MSP the old
    // stack frame is gone, so nothing here may depend on a local.
    __set_MSP(*(uint32_t const *)SYSTEM_MEMORY_BASE);
    ((void (*)(void)) * (uint32_t const *)(SYSTEM_MEMORY_BASE + 4))();
}

void usb_device_init(void)
{
    usb_clock_init();
    usb_gpio_init();

    // TinyUSB defers ISR work onto a queue, so OTG_FS must sit at or below the
    // priority ceiling that permits FromISR calls.
    HAL_NVIC_SetPriority(OTG_FS_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY, 0);

    xTaskCreate(usb_device_task, "usbd", USB_TASK_STACK_SIZE, NULL, USB_TASK_PRIORITY, NULL);
}

static void usb_device_task(void *params)
{
    (void)params;

    // tusb_init() unmasks the OTG_FS interrupt, and that ISR uses RTOS queue
    // APIs, so it must not run until the scheduler is up.
    tusb_rhport_init_t const dev_init = {.role = TUSB_ROLE_DEVICE, .speed = TUSB_SPEED_FULL};
    tusb_init(BOARD_TUD_RHPORT, &dev_init);

    while (1)
    {
        // Blocks on the event queue until the OTG_FS ISR posts something.
        tud_task();
    }
}

void tud_dfu_runtime_reboot_to_dfu_cb(void)
{
    bootloader_magic = BOOTLOADER_MAGIC;

    // TinyUSB has only queued the DETACH status stage, so give it time to reach
    // the host before this device stops answering.
    vTaskDelay(pdMS_TO_TICKS(10));

    // The descriptor sets bitWillDetach, which promises the host we take
    // ourselves off the bus rather than waiting for it to issue a bus reset.
    tud_disconnect();
    vTaskDelay(pdMS_TO_TICKS(50));

    NVIC_SystemReset();
}
