#include "M24256.h"

void M24256::init()
{
    // isConnected = i2c->isDeviceReady(address);
    if (!isConnected)
    {
        error_handler(HAL_ERROR);
    }
}

/**
 * @brief Write a single byte to the EEPROM
 *
 * @param memAddress each byte can be accessed using a 16-bit address (range 0x0000 to 0x7FFF)
 * @param data
 */
void M24256::writeByte(uint16_t memAddress, uint8_t data)
{
    uint8_t buffer[3];
    buffer[0] = (uint8_t)(memAddress >> 8);   // MSB of memory address
    buffer[1] = (uint8_t)(memAddress & 0xFF); // LSB of memory address
    buffer[2] = data;                         // data to write

    i2c->write(address, buffer, 3);
    vTaskDelay(pdMS_TO_TICKS(6));
}


/**
 * @brief Read a single byte from the EEPROM
 *
 * @param memAddress
 * @param data
 */
uint8_t M24256::readByte(uint16_t memAddress)
{
    uint8_t buffer[2];
    buffer[0] = (uint8_t)(memAddress >> 8);   // MSB of memory address
    buffer[1] = (uint8_t)(memAddress & 0xFF); // LSB of memory address

    i2c->write(address, buffer, 2);
    i2c->read(address, buffer, 1);
    return buffer[0];
}


/**
 * @brief This input signal is useful for protecting the entire contents of the memory from inadvertent write operations. Write
 * operations are disabled to the entire memory array when write control (WC) is driven high. Write operations are
 * enabled when write control (WC) is either driven low or left floating.
 * When write control (WC) is driven high, device select and address bytes are acknowledged, data bytes are not acknowledged.
 *
 * @param state
 */
void M24256::setWriteControl(bool state)
{
    writeControlPin.write(state);
}

/**
 * @brief Mass erase the entire EEPROM by writing 0xFF to all memory locations
 * 
 * This function writes 0xFF to all memory locations in the EEPROM.
 * It uses the page write capability to optimize the process.
 * 
 * @return HAL_StatusTypeDef Status of the operation
 */
HAL_StatusTypeDef M24256::massErase()
{
    // Make sure write control is enabled
    setWriteControl(false);
    
    // Create a buffer of 0xFF for page writing
    uint8_t eraseBuffer[M24256_PAGE_SIZE];
    for (int i = 0; i < M24256_PAGE_SIZE; i++) {
        eraseBuffer[i] = 0xFF;
    }
    
    // Write 0xFF to all pages
    for (uint16_t pageAddr = 0; pageAddr < M24256_TOTAL_SIZE; pageAddr += M24256_PAGE_SIZE)
    {
        // Prepare the address bytes
        uint8_t addrBuffer[2];
        addrBuffer[0] = (uint8_t)(pageAddr >> 8);   // MSB of memory address
        addrBuffer[1] = (uint8_t)(pageAddr & 0xFF); // LSB of memory address
        
        // Write the address
        HAL_StatusTypeDef status = i2c->write(address, addrBuffer, 2);
        if (status != HAL_OK)
        {
            return status;
        }
        
        // Write the page of 0xFF
        status = i2c->write(address, eraseBuffer, M24256_PAGE_SIZE);
        if (status != HAL_OK)
        {
            return status;
        }
        
        // Wait for write cycle to complete (tW)
        HAL_Delay(M24256_WRITE_CYCLE_TIME_MS);
    }
    
    return HAL_OK;
}