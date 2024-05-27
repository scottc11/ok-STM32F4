#pragma once

#include <stdarg.h>
#include "usb_device.h"
#include "usbd_cdc_if.h"

class Logger {
public:
    Logger(){};

    void init();
    void log(const char *string, ...);
    void log(int number);
    void log(float number);
    void logSystemStatus();
};