#include "tusb.h"
#include "stm32f4xx.h"

#define USB_VID 0x0483
#define USB_PID 0x5740
#define USB_BCD 0x0200

//--------------------------------------------------------------------
// Device descriptor
//--------------------------------------------------------------------

// Composite device, so the class triple must be the IAD "misc" one or Windows
// binds the whole device to the first interface's driver.
static tusb_desc_device_t const desc_device = {
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = USB_BCD,
    .bDeviceClass       = TUSB_CLASS_MISC,
    .bDeviceSubClass    = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol    = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = USB_VID,
    .idProduct          = USB_PID,
    .bcdDevice          = 0x0100,

    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,

    .bNumConfigurations = 0x01,
};

uint8_t const *tud_descriptor_device_cb(void)
{
    return (uint8_t const *)&desc_device;
}

//--------------------------------------------------------------------
// Configuration descriptor
//--------------------------------------------------------------------

enum
{
    ITF_NUM_CDC = 0,
    ITF_NUM_CDC_DATA,
    ITF_NUM_MIDI,
    ITF_NUM_MIDI_STREAMING,
    ITF_NUM_DFU_RT,
    ITF_NUM_TOTAL
};

// OTG_FS has 6 IN and 6 OUT endpoints including EP0, and 1280 bytes of FIFO RAM
// shared across all of them.
#define EPNUM_CDC_NOTIF 0x81
#define EPNUM_CDC_OUT   0x02
#define EPNUM_CDC_IN    0x82
#define EPNUM_MIDI_OUT  0x03
#define EPNUM_MIDI_IN   0x83

#define CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN + TUD_MIDI_DESC_LEN + TUD_DFU_RT_DESC_LEN)

static uint8_t const desc_configuration[] = {
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_SELF_POWERED, 100),

    TUD_CDC_DESCRIPTOR(ITF_NUM_CDC, 4, EPNUM_CDC_NOTIF, 8, EPNUM_CDC_OUT, EPNUM_CDC_IN, 64),

    TUD_MIDI_DESCRIPTOR(ITF_NUM_MIDI, 5, EPNUM_MIDI_OUT, EPNUM_MIDI_IN, 64),

    // WILL_DETACH: the device reboots itself into the bootloader on DFU_DETACH
    // rather than waiting for the host to issue a USB reset.
    TUD_DFU_RT_DESCRIPTOR(ITF_NUM_DFU_RT, 6, DFU_ATTR_CAN_DOWNLOAD | DFU_ATTR_WILL_DETACH, 1000, 1024),
};

uint8_t const *tud_descriptor_configuration_cb(uint8_t index)
{
    (void)index;
    return desc_configuration;
}

//--------------------------------------------------------------------
// String descriptors
//--------------------------------------------------------------------

enum
{
    STRID_LANGID = 0,
    STRID_MANUFACTURER,
    STRID_PRODUCT,
    STRID_SERIAL,
    STRID_CDC,
    STRID_MIDI,
    STRID_DFU_RT,
};

static char const *string_desc_arr[] = {
    [STRID_LANGID]       = (const char[]){0x09, 0x04}, // English (0x0409)
    [STRID_MANUFACTURER] = "OK200",
    [STRID_PRODUCT]      = "OK200 System Clock",
    [STRID_SERIAL]       = NULL, // generated from the MCU unique ID
    [STRID_CDC]          = "OK200 Serial",
    [STRID_MIDI]         = "OK200 MIDI",
    [STRID_DFU_RT]       = "OK200 DFU",
};

static uint16_t desc_str_buf[32];

// Renders the 96-bit MCU unique ID as 24 hex characters directly into the
// UTF-16 response buffer.
static uint8_t serial_to_utf16(uint16_t *buf)
{
    volatile uint32_t const *uid = (volatile uint32_t const *)UID_BASE;
    char const hex[] = "0123456789ABCDEF";
    uint8_t chr_count = 0;

    for (uint8_t word = 0; word < 3; word++)
    {
        uint32_t value = uid[word];
        for (int8_t nibble = 7; nibble >= 0; nibble--)
        {
            buf[chr_count++] = hex[(value >> (nibble * 4)) & 0x0F];
        }
    }

    return chr_count;
}

uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
    (void)langid;
    uint8_t chr_count;

    if (index == STRID_LANGID)
    {
        memcpy(&desc_str_buf[1], string_desc_arr[STRID_LANGID], 2);
        chr_count = 1;
    }
    else if (index == STRID_SERIAL)
    {
        chr_count = serial_to_utf16(&desc_str_buf[1]);
    }
    else
    {
        if (index >= TU_ARRAY_SIZE(string_desc_arr))
        {
            return NULL;
        }

        char const *str = string_desc_arr[index];
        chr_count = (uint8_t)strlen(str);

        // Leave room for the 2 byte header.
        uint8_t const max_count = TU_ARRAY_SIZE(desc_str_buf) - 1;
        if (chr_count > max_count)
        {
            chr_count = max_count;
        }

        for (uint8_t i = 0; i < chr_count; i++)
        {
            desc_str_buf[1 + i] = str[i];
        }
    }

    desc_str_buf[0] = (uint16_t)((TUSB_DESC_STRING << 8) | (2 * chr_count + 2));

    return desc_str_buf;
}
