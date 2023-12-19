/**
 * @file    MPR121.h
 * @brief   Device driver - MPR121 capactiive touch IC
 * @author  Scott Campbell
 * @version 1.0

******************************************************

#include <mbed.h>
#include "IS31FL3736.h"
#include "MPR121.h"

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

  thread.start(callback(&queue, &EventQueue::dispatch_forever));

  tp1.init();
  tp1.attachInterruptCallback(queue.event(callback(&tp1, &MPR121::handleTouch)));
  tp1.attachCallbackTouched(callback(onTouch));
  tp1.attachCallbackReleased(callback(onRelease));
  tp1.enable();


  while(1) {
    if (tp1.wasTouched()) {
      tp1.handleTouch();
    }
  }
}

******************************************************
*/

#ifndef __MPR121_H
#define __MPR121_H

#include "common.h"
#include "I2C.h"
#include "Callback.h"
#include "InterruptIn.h"

#define MPR121_NO_PADS_TOUCHED 0xFF

/**
 *  @class MPR121
 *  @brief API for the MPR121 capacitive touch IC
 */
class MPR121 {
public:
    /**
     *  @enum MPR121_ADDR
     *  @brief Possible terminations for the ADDR pin
     */
    enum MPR121_ADDR
    {
        ADDR_VSS = 0x5A, /*!< ADDR connected to VSS */
        ADDR_VDD,        /*!< ADDR connected to VDD */
        ADDR_SDA,        /*!< ADDR connected to SDA */
        ADDR_SCL         /*!< ADDR connected to SCL */
    };

    struct TouchedNode {
      uint8_t pad;
      struct TouchedNode *next;
      struct TouchedNode *prev;
    };

    MPR121(I2C *i2c_ptr, PinName irq_pin, MPR121_ADDR i2c_addr = ADDR_VSS) : irq(irq_pin, PullUp) { // pull-up?
        address = i2c_addr << 1;
        i2c = i2c_ptr;
    }

    void init(void);
    void poll();
    void enable(void);
    void disable(void);
    void reset();
    bool connected();
    bool padIsTouched();
    bool padIsTouched(uint8_t pad);

    uint16_t handleTouch();
    uint16_t readPads();
    uint16_t getCurrTouched();
    uint16_t getPrevTouched();
    uint8_t getLastTouchedNode();
    bool interruptDetected();
    int readInterruptPin();

    void clearInterrupt();
    void attachInterruptCallback(Callback<void()> func);
    void attachCallbackTouched(Callback<void(uint8_t pad)> func);
    void attachCallbackReleased(Callback<void(uint8_t pad)> func);

private:
  char address; // Note: The Arm Mbed API uses 8 bit addresses, so make sure to left-shift 7 bit addresses by 1 bit before passing them.
  I2C *i2c;     // use an #ifdef macro here to determin which underlying framework I2C class should be used

  InterruptIn irq;
  volatile uint16_t _button;
  volatile uint32_t _button_has_changed;

  volatile bool interrupt{false};
  uint16_t currTouched{0};
  uint16_t prevTouched{0};
  
  TouchedNode *touchedHead{NULL}; // Linked List to record the sequence / order the pads were touched

  TouchedNode *createNode(uint8_t pad);
  void addNode(struct TouchedNode **head, uint8_t pad);
  void removeNode(struct TouchedNode **head, uint8_t pad);
  TouchedNode *getNode(struct TouchedNode **head, uint8_t pad);
  void freeList(struct TouchedNode *head);

  Callback<void(uint8_t pad)> touchedCallback;
  Callback<void(uint8_t pad)> releasedCallback;
  Callback<void()> interruptCallback;

  void irq_handler(void);

  /** Write to a register
   *  Note: most writes are only valid in stop mode
   *  @param reg - The register to be written
   *  @param data - The data to be written
   */
  void writeRegister(char reg, char _data1, char _data2)
  {
    uint8_t commands[3];
    commands[0] = reg;
    commands[1] = _data1;
    commands[2] = _data2;

    i2c->write(address, commands, 3);
  }

  void writeRegister(char reg, char _data1)
  {
    uint8_t commands[2];
    commands[0] = reg;
    commands[1] = _data1;

    i2c->write(address, commands, 2);
  }

  /** Read from a register
   * After power on reset (POR) or soft reset by command, all registers are in reset default initial value (see Table 2). All the registers, except registers 0x5C (default 0x10) and 0x5D (default 0x24), are cleared.
   * Registers 0x2B ~ 0x7F are control and configuration registers which need to be correctly configured before any capacitance measurement and touch detection.
   * Registers 0x00 ~ 0x2A are output registers updating periodically by the MPR121 in Run Mode. Among these output registers, Baseline Value Registers 0x1D ~ 0x2A are also writable, this is sometimes useful when user specific baseline values are desired.
   * The MPR121’s Run Mode and Stop Mode are controlled by control bits in Electrode Configuration Register (ECR, 0x5E). When all ELEPROX_EN and ELE_EN bits are zeros, the MPR121 is in Stop Mode. While in Stop Mode, there are no capacitance or touch detection measurement on any of the 13 channels. When any of the ELEPROX_EN and ELE_EN bits are set to ‘1’, the MPR121 is in Run Mode. The MPR121 will continue to run on its own until it is set again to Stop Mode by the user.
   * The MPR121 registers read operation can be done at any time, either in Run Mode or in Stop Mode. However, the register write operation can only be done in Stop Mode. The ECR (0x5E) and GPIO/LED control registers (0x73~0x7A) can be written at anytime.
   *  @param reg - The register to read from
   *  @return The register contents
   */
  char readRegister(char reg)
  {
    uint8_t buffer[2];
    buffer[0] = reg;
    i2c->write(address, buffer, 1);
    i2c->read(address, buffer, 1);
    return buffer[0];
  }

  uint16_t readRegister16(char reg)
  {
    uint8_t buffer[2];
    buffer[0] = reg;
    i2c->write(address, buffer, 1, true);
    i2c->read(address, buffer, 2);
    return two8sTo16(buffer[0], buffer[1]);
  }
  /**
   *  @enum MPR121_REGISTER
   *  @brief The device register map
   */
  enum MPR121_REGISTER
  {
    ELE0_7_STAT = 0x00, // ELE0 - ELE7 Touch Status
    ELE8_11_STAT,       // ELE8 - ELE11, ELEPROX Touch Status
    ELE0_7_OOR_STAT,    // ELE0-7 OOR Status
    ELE8_11_OOR_STAT,   // ELE8-11, ELEPROX OOR Status

    EFD0LB, // ELE0 Electrode Filtered Data LSB
    EFD0HB, // ELE0 Electrode Filtered Data MSB
    EFD1LB,
    EFD1HB,
    EFD2LB,
    EFD2HB,
    EFD3LB,
    EFD3HB,
    EFD4LB,
    EFD4HB,
    EFD5LB,
    EFD5HB,

    EFD6LB = 0x10,
    EFD6HB,
    EFD7LB,
    EFD7HB,
    EFD8LB,
    EFD8HB,
    EFD9LB,
    EFD9HB,
    EFD10LB,
    EFD10HB,
    EFD11LB,
    EFD11HB,
    EFDPROXLB, // ELEPROX Electrode Filtered Data LSB
    EFDPROXHB, // ELEPROX Electrode Filtered Data MSB

    E0BV, // ELE0 Baseline Value
    E1BV,
    E2BV = 0x20,
    E3BV,
    E4BV,
    E5BV,
    E6BV,
    E7BV,
    E8BV,
    E9BV,
    E10BV,
    E11BV,
    EPROXBV, // ELEPROX Baseline Value

    MHDR,        // MHD Rising
    NHDR,        // NHD Amount Rising
    NCLR,        // NCL Rising
    FDLR,        // FDL Rising
    MHDF,        // MHD Falling
    NHDF = 0x30, // NHD Amount Falling
    NCLF,        // NCL Falling
    FDLF,        // FDL Falling
    NHDT,        // NHD Amount Touched
    NCLT,        // NCL Touched
    FDLT,        // FDL Touched

    MHDPROXR,        // ELEPROX MHD Rising
    NHDPROXR,        // ELEPROX NHD Amount Rising
    NCLPROXR,        // ELEPROX NCL Rising
    FDLPROXR,        // ELEPROX FDL Rising
    MHDPROXF,        // ELEPROX MHD Falling
    NHDPROXF,        // ELEPROX NHD Amount Falling
    NCLPROXF,        // ELEPROX NCL Falling
    FDLPROXF,        // ELEPROX FDL Falling
    NHDPROXT,        // ELEPROX NHD Amount Touched
    NCLPROXT,        // ELEPROX NCL Touched
    FDLPROXT = 0x40, // ELEPROX FDL Touched

    E0TTH, // ELE0 Touch Threshold
    E0RTH, // ELE0 Release Threshold
    E1TTH,
    E1RTH,
    E2TTH,
    E2RTH,
    E3TTH,
    E3RTH,
    E4TTH,
    E4RTH,
    E5TTH,
    E5RTH,
    E6TTH,
    E6RTH,
    E7TTH,
    E7RTH = 0x50,
    E8TTH,
    E8RTH,
    E9TTH,
    E9RTH,
    E10TTH,
    E10RTH,
    E11TTH,
    E11RTH,
    EPROXTTH, // ELEPROX Touch Threshold
    EPROXRTH, // ELEPROX Release Threshold

    DT_DR,             // Debounce Touch & Release
    CDC_CONFIG,        // Filter/Global CDC Configuration
    CDT_CONFIG = 0x5D, // Filter/Global CDT Configuration
    ECR,               // Electrode Configuration

    CDC0, // ELE0 Electrode Current
    CDC1 = 0x60,
    CDC2,
    CDC3,
    CDC4,
    CDC5,
    CDC6,
    CDC7,
    CDC8,
    CDC9,
    CDC10,
    CDC11,
    CDCPROX, // ELEPROX Electrode Current

    CDT0_CDT1, // ELE0, ELE1 Charge Time
    CDT2_CDT3,
    CDT4_CDT5,
    CDT6_CDT7,
    CDT8_CDT9 = 0x70,
    CDT10_CDT11,
    CDTPROX, // ELEPROX Charge Time

    GPIO_CTRL0,  // GPIO Control Register 0
    GPIO_CTRL1,  // GPIO Control Register 1
    GPIO_DATA,   // GPIO Data Register
    GPIO_DIR,    // GPIO Direction Register
    GPIO_EN,     // GPIO Enable Register
    GPIO_SET,    // GPIO Data Set Register
    GPIO_CLR,    // GPIO Data Clear Register
    GPIO_TOGGLE, // GPIO Data Toggle Register

    AUTO_CFG0, // AUTO-CONFIG Control Register 0
    AUTO_CFG1, // AUTO-CONFIG Control Register 1
    USL,       // AUTO-CONFIG USL Register
    LSL,       // AUTO-CONFIG LSL Register
    TL,        // AUTO-CONFIG Target Level Register

    SRST = 0x80 // Soft Reset Register
  };
};

#endif

