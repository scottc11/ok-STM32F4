#include "M24256.h"

struct M24256WriteJob
{
    M24256 *device;
    uint16_t address;
    uint16_t length;
    uint8_t data[1];
};

static QueueHandle_t task_M24256_write_queue = nullptr;
static TaskHandle_t task_M24256_write_handle = nullptr;

static void task_M24256_start();

void M24256::init()
{
    // isConnected = i2c->isDeviceReady(address);
    if (!isConnected)
    {
        OK_ERROR_HANDLER(HAL_ERROR, "M24256 not connected");
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

void M24256::writePage(uint16_t address, uint8_t *data, uint8_t length)
{
    (void)writeBuffer(address, data, length);
}

HAL_StatusTypeDef M24256::writeBuffer(uint16_t address, const uint8_t *data, uint16_t length)
{
    // Nothing to write or invalid pointer – treat as success and return
    if (length == 0 || data == nullptr)
        return HAL_OK;

    HAL_StatusTypeDef status = HAL_OK;
    uint16_t memAddress = address;  // current EEPROM write address
    uint16_t remaining = length;    // how many bytes we still need to write
    uint16_t dataIndex = 0;         // index into the caller-provided data buffer

    // Write until we've sent all requested bytes
    while (remaining > 0)
    {
        // Calculate how many bytes we can still fit into the current EEPROM page
        uint8_t pageOffset = memAddress % M24256_PAGE_SIZE;
        uint8_t pageRoom = M24256_PAGE_SIZE - pageOffset;
        // Chunk is the number of bytes we will write in this iteration
        uint8_t chunk = (remaining < pageRoom) ? remaining : pageRoom;
        if (chunk > M24256_PAGE_SIZE)
            chunk = M24256_PAGE_SIZE;

        // Local I2C buffer: [addr_high, addr_low, data0, data1, ...]
        uint8_t buffer[2 + M24256_PAGE_SIZE];
        // 16-bit EEPROM memory address split into two bytes
        buffer[0] = static_cast<uint8_t>(memAddress >> 8);
        buffer[1] = static_cast<uint8_t>(memAddress & 0xFF);
        // Copy the next `chunk` bytes from the source data into the transmit buffer
        memcpy(&buffer[2], &data[dataIndex], chunk);

        // Perform the actual I2C write of address + data payload
        status = i2c->write(this->address, buffer, 2 + chunk);
        if (status != HAL_OK)
            return status;

        // Wait for the EEPROM internal write cycle to complete
        vTaskDelay(pdMS_TO_TICKS(M24256_WRITE_CYCLE_TIME_MS));

        // Advance our position in both the EEPROM and the source buffer
        memAddress += chunk;
        dataIndex += chunk;
        remaining -= chunk;
    }

    return status;
}

HAL_StatusTypeDef M24256::writeBufferAsync(uint16_t address, const uint8_t *data, uint16_t length)
{
    if (length == 0 || data == nullptr)
        return HAL_OK;

    task_M24256_start();
    if (task_M24256_write_queue == nullptr)
        return HAL_ERROR;

    // "job" is heap‑allocated with pvPortMalloc, so its lifetime extends beyond the function.
    // That means the memory stays valid until the task frees it.
    size_t alloc_size = sizeof(M24256WriteJob) + length - 1;
    M24256WriteJob *job = static_cast<M24256WriteJob *>(pvPortMalloc(alloc_size));
    if (job == nullptr)
        return HAL_ERROR;

    job->device = this;
    job->address = address;
    job->length = length;
    memcpy(job->data, data, length); // copy sequence data to job buffer

    if (xQueueSend(task_M24256_write_queue, &job, 0) != pdPASS)
    {
        vPortFree(job);
        return HAL_BUSY;
    }

    return HAL_OK;
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
 * @brief Read a 16-bit value from the EEPROM using two 8-bit reads
 * 
 * @param memAddress the address to read from
 * @return the 16-bit value read from the EEPROM
 */
uint16_t M24256::readByte16(uint16_t memAddress)
{
    uint8_t buffer[2];
    buffer[0] = (uint8_t)(memAddress >> 8);   // MSB of memory address
    buffer[1] = (uint8_t)(memAddress & 0xFF); // LSB of memory address

    i2c->write(address, buffer, 2);
    i2c->read(address, buffer, 2);
    return two8sTo16(buffer[0], buffer[1]);
}

/**
 * @brief Read a buffer of data from the EEPROM (blocking)
 *
 * @param memAddress
 * @param buffer
 * @param length
 */
HAL_StatusTypeDef M24256::readBuffer(uint16_t memAddress, uint8_t *buffer, uint16_t length)
{
    uint8_t addr[2];
    addr[0] = (uint8_t)(memAddress >> 8);   // MSB of memory address
    addr[1] = (uint8_t)(memAddress & 0xFF); // LSB of memory address

    i2c->write(address, addr, 2);
    return i2c->read(address, buffer, length);
}

/**
 * @brief Read a buffer of data from the EEPROM (non-blocking)
 * 
 * @param memAddress
 * @param buffer
 * @param length
 */
HAL_StatusTypeDef M24256::readBufferAsync(uint16_t memAddress, uint8_t *buffer, uint16_t length)
{
    static uint8_t write_buffer[2];
    write_buffer[0] = (uint8_t)(memAddress >> 8);   // MSB of memory address
    write_buffer[1] = (uint8_t)(memAddress & 0xFF); // LSB of memory address

    i2c->write(address, write_buffer, 2);

    // submit an asynchronous receive request
    I2CRequest req{RequestType::Receive, i2c, this->address, buffer, length, nullptr, pdFAIL};
    return (i2c_submit_async(req) == pdPASS) ? HAL_OK : HAL_BUSY;
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

HAL_StatusTypeDef M24256::massEraseAsync()
{
    task_M24256_start();

    if (task_M24256_write_queue == nullptr)
        return HAL_ERROR;

    // "job" is heap‑allocated with pvPortMalloc, so its lifetime extends beyond the function.
    // That means the memory stays valid until the task frees it.
    size_t alloc_size = sizeof(M24256WriteJob) + M24256_TOTAL_SIZE - 1;
    M24256WriteJob *job = static_cast<M24256WriteJob *>(pvPortMalloc(alloc_size));
    if (job == nullptr)
        return HAL_ERROR;
    
    job->device = this;
    job->address = 0;
    job->length = M24256_TOTAL_SIZE;
    memset(job->data, 0xFF, M24256_TOTAL_SIZE);

    if (xQueueSend(task_M24256_write_queue, &job, 0) != pdPASS)
    {
        vPortFree(job);
        return HAL_BUSY;
    }

    return HAL_OK; // job is now enqueued, return immediately
}

/*
Dedicated EEPROM write task
Uses writeBufferAsync() to enqueue bulk writes. The task performs page-split writes using the I2C manager’s synchronous enqueue helper and enforces the per‑page write‑cycle delay so other I2C users can share the bus safely.

Each page write is followed by vTaskDelay(M24256_WRITE_CYCLE_TIME_MS), matching the EEPROM’s required write‑cycle time.
writeBufferAsync() now copies the payload into a heap‑allocated job and enqueues it, returning immediately with HAL_OK or HAL_BUSY.
*/
static void task_M24256_bulk_write(void *pvParameters)
{
    UNUSED(pvParameters);

    if (task_M24256_write_queue == nullptr)
    {
        task_M24256_write_queue = xQueueCreate(2, sizeof(M24256WriteJob *));
    }

    while (1)
    {
        M24256WriteJob *job = nullptr;
        if (xQueueReceive(task_M24256_write_queue, &job, portMAX_DELAY) != pdPASS || job == nullptr)
            continue;

        // job flags to track the status of the async write
        job->device->asyncWriteInProgress = true;
        job->device->asyncWriteFailure = false;
        job->device->asyncWriteStatus = HAL_OK;
        
        uint16_t memAddress = job->address;
        uint16_t remaining = job->length;
        uint16_t dataIndex = 0;

        while (remaining > 0)
        {

            uint8_t pageOffset = memAddress % M24256_PAGE_SIZE;
            uint8_t pageRoom = M24256_PAGE_SIZE - pageOffset;
            uint8_t chunk = (remaining < pageRoom) ? remaining : pageRoom;
            if (chunk > M24256_PAGE_SIZE)
                chunk = M24256_PAGE_SIZE;

            uint8_t buffer[2 + M24256_PAGE_SIZE];
            buffer[0] = static_cast<uint8_t>(memAddress >> 8);
            buffer[1] = static_cast<uint8_t>(memAddress & 0xFF);
            memcpy(&buffer[2], &job->data[dataIndex], chunk);

            // Submit the I2C transaction and wait for it to complete
            // Timeout is 100ms to allow for the EEPROM write cycle to complete
            HAL_StatusTypeDef status = i2c_submit_sync_tx(job->device->i2c, job->device->address, buffer, 2 + chunk, pdMS_TO_TICKS(100));

            // If the I2C transaction fails, break out of the loop
            if (status != HAL_OK)
            {
                job->device->asyncWriteFailure = true;
                job->device->asyncWriteStatus = status;
                break;
            }

            // A small delay is required to allow for the M24256 internal write cycle to complete. 
            // Issuing another write too quickly can cause the write to fail.
            vTaskDelay(pdMS_TO_TICKS(M24256_WRITE_CYCLE_TIME_MS));

            memAddress += chunk;
            dataIndex += chunk;
            remaining -= chunk;
        }

        job->device->asyncWriteInProgress = false;
        
        // notify the caller that the write is complete
        if (job->device->writeCompleteCallback)
            job->device->writeCompleteCallback();

        vPortFree(job); // free the job from the heap
    }
}

/**
 * @brief Start the EEPROM write task
 * 
 * This function creates the EEPROM write task if it doesn't already exist.
 * 
 * @return void
 */
static void task_M24256_start()
{
    if (task_M24256_write_handle == nullptr)
    {
        xTaskCreate(
            task_M24256_bulk_write,
            "eeprom_wr",
            configMINIMAL_STACK_SIZE + 128,
            nullptr,
            tskIDLE_PRIORITY + 1, // is this high or low priority?
            &task_M24256_write_handle);
    }
}