#pragma once

//--------------------------------------------------------------------
// Board / port
//--------------------------------------------------------------------

#define CFG_TUSB_MCU            OPT_MCU_STM32F4
#define CFG_TUSB_OS             OPT_OS_FREERTOS
#define CFG_TUSB_DEBUG          0

// OTG_FS is rhport 0 on STM32.
#define BOARD_TUD_RHPORT        0

#define CFG_TUD_ENABLED         1
#define CFG_TUH_ENABLED         0
#define CFG_TUD_MAX_SPEED       OPT_MODE_FULL_SPEED

// PA9 is not wired to VBUS on this board, so the device must report itself as
// self-powered/always-attached rather than waiting for a VBUS session request.
#define CFG_TUD_VBUS_DETECT_HW  0

#define CFG_TUD_ENDPOINT0_SIZE  64

//--------------------------------------------------------------------
// Device classes
//--------------------------------------------------------------------

#define CFG_TUD_CDC             1
#define CFG_TUD_MIDI            1
#define CFG_TUD_DFU_RUNTIME     1

// OTG_FS has only 1280 bytes of FIFO RAM shared across all endpoints, so keep
// the per-class buffers at a single full-speed bulk packet.
#define CFG_TUD_CDC_RX_BUFSIZE  64
#define CFG_TUD_CDC_TX_BUFSIZE  64
#define CFG_TUD_CDC_EP_BUFSIZE  64

#define CFG_TUD_MIDI_RX_BUFSIZE 64
#define CFG_TUD_MIDI_TX_BUFSIZE 64
#define CFG_TUD_MIDI_EP_BUFSIZE 64
