
# Example
```
#include <mbed.h>
#include "IS31FL3736.h"
#include "MPR121.h"

I2C i2c(I2C_SDA, I2C_SCL); //  D4, D5

IS31FL3736 leds(&i2c);
MPR121 touchPads(&i2c, D2);

void onTouch(uint8_t pad)
{
  leds.setLED(0, pad, true);
}

void onRelease(uint8_t pad)
{
  leds.setLED(0, pad, false);
}

int main() {

  leds.init();
  leds.allLEDsOff();

  touchPads.init();
  touchPads.attachCallbackTouched(callback(onTouch));
  touchPads.attachCallbackReleased(callback(onRelease));
  touchPads.enable();
  
  while(1) {

    if (touchPads.wasTouched()) {
      touchPads.handleTouch();
    }
  }
}
```