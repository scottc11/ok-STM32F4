#pragma once

#include <stdint.h>
#include "MIDI.h"
#include "task_CAN_manager.h"

/**
 * MIDI over CAN.
 *
 * One standard (11-bit) CAN frame carries one MIDI message:
 *   id          source device (OK_CAN_ID_*)
 *   data[0]     MIDI status byte, channel in the low nibble for channel messages
 *   data[1]     first data byte when the status requires it (bit 7 clear)
 *   data[2]     second data byte when the status requires it (bit 7 clear)
 *   len         1, 2, or 3, and it must match the status byte
 *   data[3..7]  unused; senders write 0 and receivers ignore them
 *
 * Each frame is a complete message. Running status is not used.
 * System exclusive and other system-common messages (0xF0–0xF7) are not part of this version.
 * A device ID that carries MIDI does not carry any other payload format.
 */

/**
 * @brief Number of bytes in a version-1 MIDI-over-CAN message, or 0 if the status is not supported.
 */
inline uint8_t midi_message_length(uint8_t status)
{
    if (status >= 0xF8) {
        return 1;
    }
    if (status < 0x80) {
        return 0;
    }

    uint8_t type = status & 0xF0;
    if (type == PROGRAM_CHANGE || type == CHANNEL_PRESSURE) {
        return 2;
    }
    if (type == NOTE_OFF || type == NOTE_ON || type == POLYPHONIC_KEY_PRESSURE
        || type == CONTROL_CHANGE || type == PITCH_BEND) {
        return 3;
    }
    return 0;
}

/**
 * @brief Validate a CAN payload and copy it into a 3-byte MIDI buffer.
 * @param data payload bytes
 * @param len number of valid payload bytes
 * @param out destination; unused bytes are cleared to 0
 * @return true when the payload is one supported MIDI message
 */
inline bool midi_can_decode(const uint8_t *data, uint8_t len, uint8_t out[3])
{
    if (data == nullptr || out == nullptr || len == 0 || len > 3) {
        return false;
    }

    uint8_t expected = midi_message_length(data[0]);
    if (expected == 0 || expected != len) {
        return false;
    }

    for (uint8_t i = 1; i < len; i++) {
        if (data[i] & 0x80) {
            return false;
        }
    }

    out[0] = data[0];
    out[1] = (len > 1) ? data[1] : 0;
    out[2] = (len > 2) ? data[2] : 0;
    return true;
}

inline bool midi_can_decode(const CANMessage &msg, uint8_t out[3])
{
    return midi_can_decode(msg.data, msg.len, out);
}

/**
 * @brief Pack one MIDI message. Status must already include the channel nibble.
 * @param status MIDI status byte
 * @param data1 first data byte (ignored when the status does not use it)
 * @param data2 second data byte (ignored when the status does not use it)
 * @param out destination for the MIDI bytes
 * @param len [out] number of bytes written to out
 * @return false when the status is unsupported or a data byte has bit 7 set
 */
inline bool midi_can_encode(uint8_t status, uint8_t data1, uint8_t data2, uint8_t out[3], uint8_t *len)
{
    uint8_t expected = midi_message_length(status);
    if (expected == 0 || out == nullptr || len == nullptr) {
        return false;
    }
    if (expected > 1 && (data1 & 0x80)) {
        return false;
    }
    if (expected > 2 && (data2 & 0x80)) {
        return false;
    }

    out[0] = status;
    out[1] = (expected > 1) ? data1 : 0;
    out[2] = (expected > 2) ? data2 : 0;
    *len = expected;
    return true;
}

/**
 * @brief Queue one MIDI message on the CAN bus.
 * @param device_id source device (OK_CAN_ID_*)
 * @param status MIDI status byte, including the channel nibble for channel messages
 * @param data1 first data byte
 * @param data2 second data byte
 */
inline BaseType_t midi_can_send(uint32_t device_id, uint8_t status, uint8_t data1, uint8_t data2, TickType_t timeout = pdMS_TO_TICKS(10))
{
    uint8_t bytes[3] = {0, 0, 0};
    uint8_t len = 0;
    if (!midi_can_encode(status, data1, data2, bytes, &len)) {
        return pdFAIL;
    }
    return can_manager_send(device_id, bytes, len, false, timeout);
}

inline BaseType_t midi_can_send_note_on(uint32_t device_id, uint8_t channel, uint8_t note, uint8_t velocity, TickType_t timeout = pdMS_TO_TICKS(10))
{
    return midi_can_send(
        device_id,
        static_cast<uint8_t>(NOTE_ON | (channel & 0x0F)),
        static_cast<uint8_t>(note & 0x7F),
        static_cast<uint8_t>(velocity & 0x7F),
        timeout);
}

inline BaseType_t midi_can_send_note_off(uint32_t device_id, uint8_t channel, uint8_t note, uint8_t velocity, TickType_t timeout = pdMS_TO_TICKS(10))
{
    return midi_can_send(
        device_id,
        static_cast<uint8_t>(NOTE_OFF | (channel & 0x0F)),
        static_cast<uint8_t>(note & 0x7F),
        static_cast<uint8_t>(velocity & 0x7F),
        timeout);
}

inline BaseType_t midi_can_send_realtime(uint32_t device_id, uint8_t status, TickType_t timeout = pdMS_TO_TICKS(10))
{
    return midi_can_send(device_id, status, 0, 0, timeout);
}
