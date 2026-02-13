---
menu: Main
---
**A lightweight and opinionated USB Full-Speed device stack for STM32 microcontrollers.**

## Overview

usbd-fs-stm32 is a bare-metal C library that implements a USB Full-Speed device stack for STM32 microcontrollers. It operates directly on the USB peripheral registers using CMSIS device headers, without depending on ST's HAL or any other middleware. The library handles all standard USB device requests internally -- enumeration, descriptor exchange, endpoint management, suspend/resume -- and exposes a callback-based interface for firmware-specific behavior.

Firmware authors define a set of callbacks to provide USB descriptors and handle data transfers. The library takes care of the rest: PMA (Packet Memory Area) buffer management, control transfer state machine, endpoint configuration, and low-power mode transitions.

## Key Highlights

- **No HAL dependency** -- operates directly on CMSIS device headers and USB peripheral registers
- **Callback architecture** -- firmware implements descriptor callbacks and data transfer handlers; the library handles all standard USB protocol logic
- **Four STM32 series supported** -- STM32C0, STM32F0, STM32G0, and STM32G4
- **Compile-time endpoint configuration** -- endpoint sizes and types are defined as preprocessor macros, with static validation of PMA memory usage
- **Descriptor type headers included** -- packed C structs and macros for standard USB, HID, Audio, and MIDI descriptors
- **BSD 3-Clause license** -- permissive open-source licensing

## Supported features

### Endpoint types

| Type | Notes |
|------|-------|
| CONTROL | Endpoint 0 only, fixed 64-byte `bMaxPacketSize0` |
| BULK | Single-buffered |
| INTERRUPT | Single-buffered |

Up to 8 endpoints (0--7) are available, with endpoint 0 always reserved for control transfers.

### Limitations

- Only one USB configuration is supported.
- No interface alternate settings.
- No isochronous endpoint support.

## Explore Further

- [Firmware Development](10_firmware-development.md) -- integration, configuration, and usage guide
- [API documentation](@@/p/usbd-fs-stm32/api/) -- function signatures, callbacks, and descriptor types
- [Source Code](https://github.com/rafaelmartins/usbd-fs-stm32) -- GitHub repository
