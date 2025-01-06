# usbd-fs-stm32

A lightweight (and very opinionated) USB FS device stack for STM32.

## Supported features

### Supporded STM32 Series

- `STM32F0`
- `STM32G4`

### USB Endpoint types

- `CONTROL` (Endpoint 0 only, 64 bytes `bMaxPacketSize0`)
- `BULK` (Single-buffered only)
- `INTERRUPT`

### Limitations

- Only one configuration possible.
- No interface alternate setting possible.


## How to use

[API documentation](https://rafaelmartins.eng.br/p/usbd-fs-stm32/api/)

TODO


## License
This code is released under a [BSD 3-Clause License](LICENSE).
