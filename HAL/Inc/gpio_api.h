#pragma once

#include "common.h"
#include "PinNames.h"
#include "PinPeripherals.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_hal_gpio_ex.h" // do you need this?
#include "stm32f4xx_ll_gpio.h"

GPIO_TypeDef *gpio_enable_clock(PinName pin);
uint32_t gpio_get_pin(PinName pin);
GPIO_TypeDef *gpio_get_port(PinName);

int gpio_get_spi_alt_mosi(PinName pin);
int gpio_get_spi_alt_miso(PinName pin);
int gpio_get_spi_alt_sclk(PinName pin);
int gpio_get_spi_alt_ssel(PinName pin);
int gpio_get_tim_alt(PinName pin, TIMName tim);

void enable_adc_pin(PinName pin);

void gpio_config_input_capture(PinName pin, TIMName tim);

void set_pin_pull(GPIO_InitTypeDef *config, PinMode mode);

void gpio_set_pin_speed(PinName pin, GPIO_Speed speed);

GPIO_PinState gpio_read_pin(PinName pin);
void gpio_irq_set(PinName pin, IrqEvent event, bool enable);
IRQn_Type gpio_get_irq_line(PinName pin);