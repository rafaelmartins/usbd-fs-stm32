# Firmware development

This guide covers integrating usbd-fs-stm32 into a firmware project, configuring endpoints, implementing the required callbacks, and using the data transfer API.

For detailed function signatures, callback prototypes, and descriptor type definitions, see the [API documentation](@@/p/usbd-fs-stm32/api/).

## Integration

### CMake setup

usbd-fs-stm32 is a CMake INTERFACE library. Add it to a firmware project using `FetchContent`:

```cmake
include(FetchContent)
FetchContent_Declare(usbd-fs-stm32
    GIT_REPOSITORY https://github.com/rafaelmartins/usbd-fs-stm32.git
    GIT_TAG main
)
FetchContent_MakeAvailable(usbd-fs-stm32)

target_link_libraries(my-firmware PRIVATE usbd-fs-stm32)
```

The library requires CMSIS device headers for the target STM32 series. These are typically provided by a build system such as [cmake-cmsis-stm32](@@/p/cmake-cmsis-stm32/), which defines the appropriate STM32 series preprocessor macro automatically.

### Header files

| Header | Purpose |
|--------|---------|
| `usbd.h` | Main library header -- public API functions and callback prototypes |
| `usb-std.h` | Standard USB descriptor types and macros (device, config, interface, endpoint, string, control request) |
| `usb-std-hid.h` | USB HID descriptor type and macros |
| `usb-std-audio.h` | USB Audio descriptor types and macros |
| `usb-std-midi.h` | USB MIDI streaming descriptor types and macros |

Include `usbd.h` for the core library API. The descriptor headers are optional and only needed when building descriptors for the corresponding USB classes.

## Configuration

All configuration is done at compile time via preprocessor macros, typically set through `target_compile_definitions` in CMake.

### Endpoint buffer sizes

Each endpoint (1--7) has independent IN and OUT buffer sizes. Setting a size to 0 (the default) disables that direction for the endpoint. Endpoint 0 is always enabled with a fixed 64-byte buffer in both directions.

| Macro | Default | Description |
|-------|---------|-------------|
| `USBD_EP1_IN_SIZE` | `0` | Endpoint 1 IN buffer size in bytes |
| `USBD_EP1_OUT_SIZE` | `0` | Endpoint 1 OUT buffer size in bytes |
| `USBD_EP2_IN_SIZE` | `0` | Endpoint 2 IN buffer size in bytes |
| `USBD_EP2_OUT_SIZE` | `0` | Endpoint 2 OUT buffer size in bytes |
| `USBD_EP3_IN_SIZE` | `0` | Endpoint 3 IN buffer size in bytes |
| `USBD_EP3_OUT_SIZE` | `0` | Endpoint 3 OUT buffer size in bytes |
| `USBD_EP4_IN_SIZE` | `0` | Endpoint 4 IN buffer size in bytes |
| `USBD_EP4_OUT_SIZE` | `0` | Endpoint 4 OUT buffer size in bytes |
| `USBD_EP5_IN_SIZE` | `0` | Endpoint 5 IN buffer size in bytes |
| `USBD_EP5_OUT_SIZE` | `0` | Endpoint 5 OUT buffer size in bytes |
| `USBD_EP6_IN_SIZE` | `0` | Endpoint 6 IN buffer size in bytes |
| `USBD_EP6_OUT_SIZE` | `0` | Endpoint 6 OUT buffer size in bytes |
| `USBD_EP7_IN_SIZE` | `0` | Endpoint 7 IN buffer size in bytes |
| `USBD_EP7_OUT_SIZE` | `0` | Endpoint 7 OUT buffer size in bytes |

### Endpoint types

Each endpoint (1--7) has a configurable transfer type. The type must match the `bmAttributes` field of the corresponding endpoint descriptor.

| Macro | Default | Valid values |
|-------|---------|--------------|
| `USBD_EP1_TYPE` | `BULK` | `BULK`, `INTERRUPT` |
| `USBD_EP2_TYPE` | `BULK` | `BULK`, `INTERRUPT` |
| `USBD_EP3_TYPE` | `BULK` | `BULK`, `INTERRUPT` |
| `USBD_EP4_TYPE` | `BULK` | `BULK`, `INTERRUPT` |
| `USBD_EP5_TYPE` | `BULK` | `BULK`, `INTERRUPT` |
| `USBD_EP6_TYPE` | `BULK` | `BULK`, `INTERRUPT` |
| `USBD_EP7_TYPE` | `BULK` | `BULK`, `INTERRUPT` |

### PMA memory budget

All endpoint buffers are allocated from the USB peripheral's Packet Memory Area (PMA). The total PMA size depends on the STM32 series:

| STM32 series | PMA size | Available for endpoints 1--7 |
|--------------|----------|------------------------------|
| STM32C0 | 2048 bytes | 1856 bytes |
| STM32F0 | 1024 bytes | 832 bytes |
| STM32G0 | 2048 bytes | 1856 bytes |
| STM32G4 | 1024 bytes | 832 bytes |

The "available" column accounts for the 64-byte buffer descriptor table and the two 64-byte endpoint 0 buffers. The library validates at compile time that the configured endpoint sizes fit within the available PMA space.

### Example

A HID device with a single 8-byte interrupt IN endpoint on endpoint 1:

```cmake
target_compile_definitions(my-firmware PRIVATE
    USBD_EP1_IN_SIZE=8
    USBD_EP1_TYPE=INTERRUPT
)
```

## Execution model

### Initialization

Call `usbd_init()` during firmware startup, after configuring the system clock. This function enables the USB peripheral clock, initializes the PMA buffer layout, configures interrupts, and enables the D+ pull-up resistor to signal device attachment to the host.

### Main loop

Call `usbd_task()` periodically from the firmware main loop or from the USB interrupt handler. This function processes all pending USB events: reset, suspend, resume, control transfers, and data transfers.

```c
int main(void)
{
    // ... system and peripheral initialization ...

    usbd_init();

    while (1) {
        usbd_task();
        // ... other firmware tasks ...
    }
}
```

When using interrupt-driven operation, configure the USB IRQ handler to call `usbd_task()` directly.

### SOF-driven IN polling

When the `usbd_in_cb` callback is defined, the library enables SOF (Start of Frame) interrupt processing. On each SOF event (every 1 ms for Full-Speed USB), the library checks all configured IN endpoints and calls `usbd_in_cb` for any endpoint that is ready to accept new data. This provides a periodic polling mechanism for IN endpoints without requiring the firmware to manage timing.

## Callbacks

The library uses a callback-based design. Firmware must implement a set of required callbacks and may optionally implement additional callbacks for handling class-specific requests, data transfers, and power management events.

### Required callbacks

Four callbacks must be defined by every firmware using the library. These provide the USB descriptors that the library sends to the host during enumeration:

- `usbd_get_device_descriptor_cb` -- returns the device descriptor
- `usbd_get_config_descriptor_cb` -- returns the configuration descriptor (including all interface, endpoint, and class-specific descriptors as a single contiguous block)
- `usbd_get_interface_descriptor_cb` -- returns an interface descriptor by interface number
- `usbd_get_string_descriptor_cb` -- returns a string descriptor by language and index

### Optional callbacks

Optional callbacks are declared with the `weak` attribute. The library checks for their presence at runtime and skips the call if they are not defined.

- `usbd_reset_hook_cb` -- called before and after USB bus reset
- `usbd_set_address_hook_cb` -- called when the host assigns a USB address (indicates successful enumeration)
- `usbd_suspend_hook_cb` -- called when the device enters USB suspend
- `usbd_resume_hook_cb` -- called when the device exits USB suspend
- `usbd_out_cb` -- called when data is received from the host on any endpoint
- `usbd_in_cb` -- called when an IN endpoint is ready to accept data
- `usbd_sof_cb` -- called on each USB Start of Frame packet
- `usbd_ctrl_request_handle_class_cb` -- called for USB class-specific control requests
- `usbd_ctrl_request_handle_vendor_cb` -- called for USB vendor-specific control requests
- `usbd_ctrl_request_get_descriptor_interface_cb` -- called for GET_DESCRIPTOR requests directed at an interface (used for HID report descriptors and similar)

## Data transfer

### Sending data (IN transfers)

Use `usbd_in()` to transmit data to the host on a given endpoint. The buffer must not exceed the configured endpoint size. For larger payloads, split the data across multiple calls, each in response to an `usbd_in_cb` notification.

`usbd_in()` may also be called outside of the `usbd_in_cb` callback to schedule data proactively, as long as no transmission is already in progress on that endpoint.

For control transfers on endpoint 0, use `usbd_control_in()` instead. This function handles multi-packet transfers automatically, splitting buffers larger than 64 bytes across multiple transactions.

### Receiving data (OUT transfers)

When the host sends data, the library calls `usbd_out_cb` with the endpoint number. The firmware then calls `usbd_out()` to read the data into a buffer. The `autoenable` parameter controls whether the endpoint is immediately re-armed for the next OUT transfer. If set to `false`, the firmware must call `usbd_out_enable()` manually when ready to receive more data.

### Serial number helper

`usbd_serial_internal_string_descriptor()` generates a USB string descriptor from the STM32's factory-programmed unique device ID. Call it from within `usbd_get_string_descriptor_cb` when handling the serial number string index.

## Source files

| File | Purpose |
|------|---------|
| `src/usbd.c` | Library implementation -- PMA management, endpoint handling, control transfer state machine, USB event processing |
| `include/usbd.h` | Public API and callback prototypes |
| `include/usb-std.h` | Standard USB descriptor types and request/descriptor macros |
| `include/usb-std-hid.h` | USB HID descriptor type and class-specific macros |
| `include/usb-std-audio.h` | USB Audio descriptor types and class-specific macros |
| `include/usb-std-midi.h` | USB MIDI streaming descriptor types and class-specific macros |
