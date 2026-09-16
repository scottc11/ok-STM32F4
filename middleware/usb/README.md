# USB device stack

This directory is the board-specific glue between the application and
[TinyUSB](https://github.com/hathach/tinyusb), which lives as a nested submodule at
`ok-STM32F4/middleware/tinyusb` pinned to release `0.21.0`.

It replaces the ST USB Device Library (`middleware/STM32_USB_Device_Library`) and the
CubeMX-generated `middleware/USB_DEVICE` app, both of which were deleted. The ST HAL PCD
driver went with them — TinyUSB's `dcd_dwc2` driver talks to the OTG_FS registers directly,
so `HAL_PCD_MODULE_ENABLED` is off in `system/Inc/stm32f4xx_hal_conf.h`.

## Files

| File | Purpose |
| --- | --- |
| `tusb_config.h` | Compile-time TinyUSB configuration. Picked up automatically because this directory is on the include path. |
| `usb_descriptors.c` | Device, configuration, and string descriptors. Implements the three `tud_descriptor_*_cb` callbacks TinyUSB requires. |
| `usb_device.c` / `.h` | Clock, GPIO, and NVIC bring-up, plus the FreeRTOS task that services the stack. Exposes a single entry point. |

## What the host sees

A composite full-speed device, VID `0x0483` / PID `0x5740` (carried over from the old ST
descriptors), with the IAD class triple (`0xEF / 0x02 / 0x01`) so Windows binds a driver per
interface rather than lumping the whole device onto the first one.

| Interface | Class | Endpoints |
| --- | --- | --- |
| 0, 1 | CDC-ACM (virtual COM port) | `0x81` notification (8 B), `0x02` OUT, `0x82` IN (64 B) |
| 2, 3 | USB-MIDI 1.0 | `0x03` OUT, `0x83` IN (64 B) |
| 4 | DFU runtime | none |

The serial number string is generated at runtime from the 96-bit MCU unique ID at `UID_BASE`,
so every board enumerates distinctly.

## How it starts

`usb_device_init()` is called once from `main()` in `firmware/Src/main.cpp`, after
`SystemClock_Config()` and before `vTaskStartScheduler()`. It does hardware bring-up and then
creates the `usbd` task; the stack itself is started from inside that task.

```
main()
  └─ usb_device_init()
       ├─ usb_clock_init()   48 MHz CLK48 from PLLSAI-P
       ├─ usb_gpio_init()    PA11/PA12 as AF10, OTG_FS peripheral clock
       ├─ NVIC priority      configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY
       └─ xTaskCreate("usbd")
            └─ usb_device_task()
                 ├─ tusb_init()   ← runs only once the scheduler is up
                 └─ for(;;) tud_task()
```

`OTG_FS_IRQHandler()` in `system/Src/stm32f4xx_it.c` forwards to `tud_int_handler(0)`. The ISR
only drains hardware state into a queue; all class callbacks run in the `usbd` task.

## Using it

Everything below is TinyUSB's normal device API — include `tusb.h` and call it from any task.
Nothing needs to go through this directory.

### Virtual COM port

```c
#include "tusb.h"

if (tud_cdc_connected()) {
    tud_cdc_write(buf, len);
    tud_cdc_write_flush();   // otherwise data sits in the FIFO until it fills
}
```

Receiving is either polled with `tud_cdc_available()` / `tud_cdc_read()`, or driven by
implementing `tud_cdc_rx_cb(uint8_t itf)`, which TinyUSB calls from the `usbd` task when a
packet lands.

### MIDI

```c
uint8_t note_on[3] = { 0x90, 0x3C, 0x7F };
tud_midi_stream_write(0, note_on, sizeof(note_on));   // 0 = virtual cable number
```

Inbound: `tud_midi_available()` with `tud_midi_stream_read()`, or `tud_midi_packet_read()` for
raw 4-byte USB-MIDI event packets.

Note this is entirely separate from the `MIDI` class in `misc/MIDI.cpp`, which is serial MIDI
over USART1.

### Connection state

Implement `tud_mount_cb()`, `tud_umount_cb()`, `tud_suspend_cb()`, or `tud_resume_cb()` as free
functions anywhere in the project; TinyUSB defines them weak in `usbd.c`.

Be aware that `tud_umount_cb()` does not fire when the cable is physically unplugged on this
board — that path depends on VBUS sensing, which is disabled below. It still fires when the
host deconfigures the device with `SET_CONFIGURATION 0`. Do not rely on it to detect a yanked
cable.

## Configuration

The knobs worth knowing about in `tusb_config.h`:

- `CFG_TUD_VBUS_DETECT_HW 0` — PA9 is not wired to VBUS on this board, so the device reports
  itself as always-attached instead of waiting for a VBUS session request. Without this it
  never enumerates.
- `CFG_TUSB_OS OPT_OS_FREERTOS` — TinyUSB uses the kernel's queues and semaphores. FreeRTOS
  headers are already on the global include path, so `CFG_TUSB_OS_INC_PATH` stays at its
  default.
- `CFG_TUSB_DEBUG 0` — raising this routes TinyUSB's logs through `printf`, which goes nowhere
  on this board. Define `CFG_TUSB_DEBUG_PRINTF` to a printf-style function backed by
  `uart_logger` before turning it up.
- The `*_BUFSIZE` values are all one full-speed bulk packet (64 bytes).

### FIFO budget

OTG_FS has 6 endpoints and only 320 words (1.25 KB) of FIFO RAM shared across all of them,
which is the tightest constraint here. The current configuration uses:

- RX FIFO (shared across all OUT endpoints): 60 words
- TX FIFOs: 16 (EP0) + 2 (CDC notify) + 16 (CDC IN) + 16 (MIDI IN) = 50 words

That is 110 of 320 words, so there is room to add another class. `dcd_dwc2` allocates this at
runtime and asserts if it runs out, so a failure here shows up as a hang in `tusb_init()`
rather than a build error.

## Notable gotchas

**`tusb_init()` must run after the scheduler starts.** It unmasks the OTG_FS interrupt, and
that ISR calls FreeRTOS `FromISR` APIs. Calling it from `main()` means a host that is already
attached can trigger the ISR before the kernel exists. This is why it lives in the task body
rather than in `usb_device_init()`.

**The OTG_FS interrupt priority is not arbitrary.** It must be numerically greater than or
equal to `configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY` (5), or FreeRTOS will assert the first
time the ISR posts an event. The old ST code ran this interrupt at priority 0, which would now
be a bug.

**`pdTICKS_TO_MS` is shimmed.** The FreeRTOS copy vendored in `middleware/FreeRTOS` predates
that macro, but TinyUSB's OSAL uses it. The standard definition is added in the
`USER CODE BEGIN Defines` section of `system/Inc/FreeRTOSConfig.h`. Delete the shim if
FreeRTOS is ever updated.

**The DFU interface cannot be flashed directly.** It is a *runtime* interface
(`bInterfaceProtocol = 0x01`, `bcdDFUVersion = 0x0110`), so it accepts only `DFU_DETACH` and
`GET_STATUS`. Pointing a DFU tool at it and hitting "download" will always fail — often with a
confusing DfuSe error, because tools tend to assume any `0x0483` device is an ST DfuSe target.
You must detach first, then flash the bootloader that appears. See the section below.

**Build layout.** `ok-STM32F4/Makefile` flattens C objects into `build/` by basename, so any
TinyUSB source added later must have a filename unique across the whole project.

## Firmware update over USB

The DFU runtime interface does not flash anything itself. It exists purely to bounce the board
into the STM32 factory bootloader in system memory, which is a real DfuSe target.

```
app running (0483:5740)
  └─ host sends DFU_DETACH to interface 4
       └─ tud_dfu_runtime_reboot_to_dfu_cb()
            ├─ writes a magic word to .noinit RAM
            ├─ tud_disconnect() so the host sees the device leave the bus
            └─ NVIC_SystemReset()
                 └─ main() → usb_device_check_bootloader_entry()
                      └─ magic matches → jump to 0x1FFF0000
                           └─ STM32 BOOTLOADER (0483:DF11, DfuSe) ready to flash
```

The reset is deliberate. Jumping straight from the `usbd` task would mean tearing down a live
RTOS, an active USB peripheral, and whatever DMA happens to be running. Going through a reset
means the jump happens on an effectively virgin machine: `usb_device_check_bootloader_entry()`
is the first statement in `main()`, before `HAL_Init()`, and every global in this project only
registers itself in its constructor rather than touching hardware. So the jump needs no
teardown at all.

The magic word lives in the `.noinit` section added to `STM32F446RETx_FLASH.ld`, placed
immediately after `.bss` so the startup zero loop (which clears `[_sbss, _ebss)`, exclusive)
leaves it alone. It is cleared before the jump, so a bootloader that fails to start cannot trap
the board in a reset loop.

Two things to expect from a browser-based tool:

- **You have to pick the device twice.** The bootloader enumerates as a different VID/PID
  (`0483:DF11`), so WebUSB treats it as a new device needing its own permission grant. Select
  "STM32 BOOTLOADER" in the picker after detaching.
- **The download target is the bootloader, not the app.** If the tool is still pointed at
  `0483:5740` you will get a DfuSe command error, because you are talking to the runtime
  interface described above.

Unchanged alternatives: `make program` flashes over ST-Link, and `make usb-upload` runs
`dfu-util` against the bootloader once the board is in it (by detach or by BOOT0).

## Updating TinyUSB

```sh
cd ok-STM32F4/middleware/tinyusb
git fetch --tags
git checkout <new-tag>
cd ../.. && git add middleware/tinyusb && git commit
```

Only `src/` is compiled — TinyUSB's `hw/`, `examples/`, and `lib/` directories are unused, and
its own submodules are deliberately left uninitialised. The exact source list is in
`ok-STM32F4/Makefile` under `TINYUSB_PATH`; upstream occasionally moves files between releases,
so check that list when bumping versions.

Because this is a submodule inside a submodule, anyone cloning a project that uses ok-STM32F4
needs `git submodule update --init --recursive`.
