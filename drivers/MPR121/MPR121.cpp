

#include "MPR121.h"

/**
 * Clear state variables and initilize the dependant objects
*/
void MPR121::init(void)
{
    // setup and registers - start with POR values (must be in stop mode)
    this->reset();

    // Baseline Filtering Control Register (changes response sensitivity)
    MPR121::writeRegister(MHDR, 0x1);  //REG 0x2B
    MPR121::writeRegister(NHDR, 0x1);  //REG 0x2C
    MPR121::writeRegister(NCLR, 0x0);  //REG 0x2D
    MPR121::writeRegister(FDLR, 0x0);  //REG 0x2E
    MPR121::writeRegister(MHDF, 0x1);  //REG 0x2F
    MPR121::writeRegister(NHDF, 0x1);  //REG 0x30
    MPR121::writeRegister(NCLF, 0xFF); //REG 0x31
    MPR121::writeRegister(FDLF, 0x2);  //REG 0x32

    // Touch / Release Threshold
    for (int i = 0; i < (12 * 2); i += 2) // touch
    {
        MPR121::writeRegister(static_cast<MPR121_REGISTER>(E0TTH + i), 0x20); //REG 0x41...0x58 odd
    }
    for (int i = 0; i < (12 * 2); i += 2) // release
    {
        MPR121::writeRegister(static_cast<MPR121_REGISTER>(E0RTH + i), 0x10); //REG 0x41...0x58 even
    }

    // Debounce Register DR=b6...4, DT=b2...0
    MPR121::writeRegister(DT_DR, 0x11); //REG 0x5B

    // Filter and Global CDC CDT Configuration (sample time, charge current)
    MPR121::writeRegister(CDC_CONFIG, 0x10); //REG 0x5C default 0x10
    MPR121::writeRegister(CDT_CONFIG, 0x20); //REG 0x5D default 0x24

    // Auto-Configuration Registers
    MPR121::writeRegister(AUTO_CFG0, 0x33); // REG 0x7B
    MPR121::writeRegister(AUTO_CFG1, 0x07); // REG 0x7C
    MPR121::writeRegister(USL, 0xc9);       // REG 0x7D((3.3-.07)/3.3) * 256
    MPR121::writeRegister(LSL, 0x83);       // REG 0x7E((3.3-.07)/3.3) * 256 * 0.65f
    MPR121::writeRegister(TL, 0xb5);        // REG 0x7F((3.3-.07)/3.3) * 256 * 0.9f
    // 255 > USL > TL > LSL > 0

    // Electrode Configuration Register - enable all 12 and start
    MPR121::writeRegister(ECR, 0x8f);

    irq.fall(callback(this, &MPR121::irq_handler)); // and attach the interrupt handler
}

void MPR121::poll() {
    if (this->interruptDetected())
    {
        this->handleTouch();
    }
}

/**
 * @brief asserts soft reset. The soft reset does not effect the I2C module, but all others reset the same as POR.
 *
 */
void MPR121::reset()
{
    this->writeRegister(SRST, 0x63);
}

/** NOTE: Not yet tested
 * Check if the IC is successfully connected to I2C line by checking a known default register value
 * NOTE: run this func before running init, as init may change the target registers value
*/
bool MPR121::connected()
{
    volatile uint8_t temp = this->readRegister(CDT_CONFIG);
    if (temp == 0x24)
    {
        return true;
    } else {
        return false;
    }
}

/**
 * Allow the IC to run and collect user input
 * The MPR121’s Run Mode and Stop Mode are controlled by control bits in Electrode Configuration Register (ECR, 0x5E).
 * When all ELEPROX_EN and ELE_EN bits are zeros, the MPR121 is in Stop Mode. While in Stop Mode, there are no
 * capacitance or touch detection measurement on any of the 13 channels.
 * When any of the ELEPROX_EN and ELE_EN bits are set to ‘1’, the MPR121 is in Run Mode.
 * The MPR121 will continue to run on its own until it is set again to Stop Mode by the user.
 *
 * The MPR121 registers read operation can be done at any time, either in Run Mode or in Stop Mode. 
 * However, the register write operation can only be done in Stop Mode. The ECR (0x5E) and GPIO/LED 
 * control registers (0x73~0x7A) can be written at anytime.
 */
void MPR121::enable(void)
{
    _button = 0;
    _button_has_changed = 0;
    // enable the 12 electrodes - allow disable to put device into lower current consumption mode
    writeRegister(ECR, 0x8f);
}

/**
 * Stop the IC and put into low power mode 
*/
void MPR121::disable(void)
{
    // detach the interrupt handler
    writeRegister(ECR, 0x0);
}

/**
 * @brief This method gets called everytime the MPR121 delivers an interrupt.
 * Calls both touch and release callbacks with the given pad that was touched.
 * 
 * @return the currently touched (or not touched) pads
*/
uint16_t MPR121::handleTouch() {
    // Get the currently touched pads
    currTouched = this->readPads();

    if (currTouched != prevTouched)
    {
        for (uint8_t i = 0; i < 12; i++)
        {
            // it if *is* touched and *wasnt* touched before, execute callback
            if (bitwise_read_bit(currTouched, i) && !bitwise_read_bit(prevTouched, i))
            {
                if (touchedCallback) touchedCallback(i);
            }

            // if it *was* touched and now *isnt*, execute callback
            if (!bitwise_read_bit(currTouched, i) && bitwise_read_bit(prevTouched, i))
            {
                if (releasedCallback) releasedCallback(i);
            }
        }

        // reset our state
        prevTouched = currTouched;
    }
    return currTouched;
}

/** Fetches currently touched pad data from MPR121 then clears class interrupt
 * @return 16 bit value containing status of all 12 pads
 * NOTE: You must clear interrupt flag before reading the IC, otherwise the callback function would 
 * execute and set interrupt to true, which would then get immediately cleared by clearInterrupt(). 
 * So the POLL function would not know to read the IC again.
*/
uint16_t MPR121::readPads()
{
    this->clearInterrupt(); 
    uint16_t touched = readRegister16(ELE0_7_STAT);
    return touched;
}

uint16_t MPR121::getCurrTouched() { return currTouched; }
uint16_t MPR121::getPrevTouched() { return prevTouched; }

/**
 * @brief returns true if any of the pads are being touched
 * 
 * @return true 
 * @return false 
 */
bool MPR121::padIsTouched()
{
    if (this->getCurrTouched() != 0x00) {
        return true;
    } else {
        return false;
    }
}

/**
 * @brief The interrupt handler for the IRQ pin
 * if callback present, executes the callback, else, sets an interrupt flag
 * 
 * NOTE: handle event queue here
 * if you call handleTouch() in this method, you would be executing unsafe IRQ code (i2c comms), so
 * you must use a thread/event queue to execute the code later.
 * Adding handleTouch to the queue could possibly be done using an additional callback function, 
 * as to build a more reusable MPR121 class
*/
void MPR121::irq_handler(void) {
    if (interruptCallback) {
        interruptCallback();
    } else {
        interrupt = true;
    }
    return;
}

bool MPR121::interruptDetected() {
    return interrupt;
}

void MPR121::clearInterrupt() {
    interrupt = false;
    return;
}

int MPR121::readInterruptPin() {
    return irq.read();
}

/**
 * @brief Attach a callback to be invoked when an interrupt is detected. MUST be ISR safe
 * @param func MBED Callback function
*/
void MPR121::attachInterruptCallback(Callback<void()> func) {
    interruptCallback = func;
}

/**
 * @brief Attach a function to be called everytime a pad is touched
 * 
 * @param func MBED Callback function which takes a single uint8_t argument
*/
void MPR121::attachCallbackTouched(Callback<void(uint8_t pad)> func)
{
    touchedCallback = func;
}

/**
 * @brief Attach a function to be called everytime a pad is released
 * 
 * @param func MBED Callback function which takes a single uint8_t argument
*/
void MPR121::attachCallbackReleased(Callback<void(uint8_t pad)> func)
{
    releasedCallback = func;
}