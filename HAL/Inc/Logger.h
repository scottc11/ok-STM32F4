#pragma once

#include <stdarg.h>
#include "usb_device.h"
#include "usbd_cdc_if.h"

class Logger {
public:
    Logger(){};

    void init();
    uint8_t log(const char *string, ...);
    uint8_t log(int number);
    uint8_t log(float number);
    uint8_t logSystemStatus();
};