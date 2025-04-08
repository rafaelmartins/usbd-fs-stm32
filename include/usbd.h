/*
 * usbd-fs-stm32: A lightweight (and very opinionated) USB FS device stack for STM32.
 *
 * SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file usbd.h
 * @brief Main @c usbd-fs-stm32 header.
 *
 * This header defines the main functions and callbacks implemented by the @c usbd-fs-stm32
 * library.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <usb-std.h>

/**
 * @brief Size of Endpoint 0 memory buffers.
 *
 * These buffers are always 64 bytes.
 *
 * When defining USB descriptors via @ref usbd_device_descriptor_t struct,
 * make sure to set @c bMaxPacketSize0 property to @c USBD_EP0_SIZE.
 */
#define USBD_EP0_SIZE 64

/**
 * @name Public API
 * Functions to be called by library consumers when implementing USB device
 * firmware.
 *
 * @{
 */

/**
 * @brief Library initializer.
 *
 * Function that initializes the USB peripheral, including the internal memory
 * buffers.
 *
 * This function must be called during firmware initialization, before entering the firmware main loop.
 */
void usbd_init(void);

/**
 * @brief Library main loop task.
 *
 * Function that runs all the operations related to the USB peripheral.
 *
 * This function must be called periodically from the firmware main loop
 * or from the USB IRQ handler (make sure to initialize the handler function
 * properly).
 */
void usbd_task(void);

/**
 * @brief Generate USB string descriptor from the internal STM32 serial number.
 * @returns A reference to an internally managed @ref usb_string_descriptor_t.
 *
 * Function that generates a USB string descriptor based on the internal
 * serial number inserted by ST during manufacturing.
 *
 * It should be called from the @ref usbd_get_string_descriptor_cb when handling
 * the request for a string descriptor with the index @c iSerialNumber, as
 * defined by the device descriptor.
 */
const usb_string_descriptor_t* usbd_serial_internal_string_descriptor(void);

/**
 * @brief Transmit data to the host in response to a USB IN request.
 * @param[in] ept    Endpoint number.
 * @param[in] buf    Pointer to a buffer containing data to be transmitted to the host.
 * @param[in] buflen Size of the @c buf buffer, in bytes.
 * @returns A boolean indicating that the data was successfully scheduled for transmission.
 *
 * The buffer should not exceed the size of the endpoint, as defined by the
 * endpoint descriptor via @ref usb_endpoint_descriptor_t. To send larger chunks
 * of data the caller must split the data and call the function multiple times in
 * response to multiple IN requests.
 *
 * This function may be called outside of the @ref usbd_in_cb callback, to schedule data
 * to be transmitted immediately, as long as there's no ongoing transmission.
 */
bool usbd_in(uint8_t ept, const void *buf, uint16_t buflen);

/**
 * @brief Receive data from the host following a USB OUT request.
 * @param[in]  ept        Endpoint number.
 * @param[out] buf        Pointer to a buffer to receive the data transmitted by the host.
 * @param[in]  buflen     Size of the @c buf buffer, in bytes.
 * @param[in]  autoenable Re-enable reception after processing the incoming data.
 * @returns The number of bytes received from the host.
 *
 * The buffer should not exceed the size of the endpoint, as defined by the
 * endpoint descriptor via @ref usb_endpoint_descriptor_t. Ideally it should be at
 * least the same size of the endpoint.
 *
 * Usually the firmware should continue accepting USB OUT requests and receive the
 * data by calling this function if the incoming data is larger than the endpoint
 * size.
 */
uint16_t usbd_out(uint8_t ept, void *buf, uint16_t buflen, bool autoenable);

/**
 * @brief Enable data reception from the host.
 * @param[in] ept Endpoint number.
 *
 * This function must be called to re-enable data reception after calling @ref usbd_out
 * with the @c autoenable argument set to false.
 */
void usbd_out_enable(uint8_t ept);

/**
 * @brief Transmit data to the host in response to a CONTROL USB IN request on endpoint 0.
 * @param[in] buf    Pointer to a buffer containing data to be transmitted to the host.
 * @param[in] buflen Size of the @c buf buffer, in bytes.
 * @param[in] reqlen Size of the CONTROL USB IN request data.
 *
 * The buffer may exceed the size of the endpoint 0 (64 bytes). The function is capable of
 * handling the transmission of the whole buffer automatically.
 *
 * @warning This function exists only because some standard requests are frequently
 * larger than the endpoint 0 size. There's no @c usbd_control_out counterpart, please
 * use @ref usbd_out instead, by passing @c 0 as the endpoint number argument.
 */
void usbd_control_in(const void *buf, uint16_t buflen, uint16_t reqlen);

/**
 * @}
 */

/**
 * @name Callbacks
 * Function callbacks that should be defined by the library consumers when implementing
 * an USB device firmware.
 *
 * @{
 */

/**
 * @brief Required callback to define USB device descriptor.
 * @returns A reference to a constant @ref usb_device_descriptor_t.
 */
const usb_device_descriptor_t* usbd_get_device_descriptor_cb(void);

/**
 * @brief Required callback to define USB configuration descriptor.
 * @returns A reference to a constant @ref usb_config_descriptor_t.
 *
 * @warning The library supports @b only @b one configuration. That is why the
 * callback does not accept any arguments!
 */
const usb_config_descriptor_t* usbd_get_config_descriptor_cb(void);

/**
 * @brief Required callback to define USB interface descriptor.
 * @param[in] itf Interface number.
 * @returns A reference to a constant @ref usb_interface_descriptor_t.
 */
const usb_interface_descriptor_t* usbd_get_interface_descriptor_cb(uint16_t itf);

/**
 * @brief Required callback to define USB string descriptor.
 * @param[in] lang The 16 bits identifier of the requested language.
 * @param[in] idx  The index of the string descriptor to be returned, as defined by the descriptor.
 * @returns A reference to a constant @ref usb_string_descriptor_t.
 */
const usb_string_descriptor_t* usbd_get_string_descriptor_cb(uint16_t lang, uint8_t idx);

/**
 * @brief Optional hook callback for USB RESET requests.
 * @param[in] before Notifies if the callback call is happening before or after the device reset.
 */
void usbd_reset_hook_cb(bool before) __attribute__((weak));

/**
 * @brief Optional hook callback for USB SET_ADDRESS control requests.
 * @param[in] addr The address assigned by the host.
 *
 * Setting the address is the last step of the device enumeration process. The device can
 * be considered as enumerated by the host when this hook is called.
 */
void usbd_set_address_hook_cb(uint8_t addr) __attribute__((weak));

/**
 * @brief Optional hook callback for USB SUSPEND requests.
 *
 * This function is responsible for taking any required actions to reduce the power consumption
 * of the device during suspension. The library will enable the internal STM32 low-power mode
 * automatically, but this only reduces the consumption of the USB peripheral itself.
 */
void usbd_suspend_hook_cb(void) __attribute__((weak));

/**
 * @brief Optional hook callback for USB RESUME requests.
 *
 * This function is responsible for taking any required actions to return the device to normal
 * execution after a suspension, usually by reversing the actions taken by the
 * @c usbd_suspend_hook_cb callback when the device entered suspension. The library will disable
 * the internal STM32 low-power mode automatically.
 */
void usbd_resume_hook_cb(void) __attribute__((weak));

/**
 * @brief Optional callback for USB OUT requests.
 * @param[in] ept Endpoint number.
 */
void usbd_out_cb(uint8_t ept) __attribute__((weak));

/**
 * @brief Optional callback for USB IN requests.
 * @param[in] ept Endpoint number.
 */
void usbd_in_cb(uint8_t ept) __attribute__((weak));

/**
 * @brief Optional callback for USB CONTROL class requests.
 * @param[in] req A reference to a @ref usb_ctrl_request_t.
 */
bool usbd_ctrl_request_handle_class_cb(usb_ctrl_request_t *req) __attribute__((weak));

/**
 * @brief Optional callback for USB CONTROL vendor requests.
 * @param[in] req A reference to a @ref usb_ctrl_request_t.
 */
bool usbd_ctrl_request_handle_vendor_cb(usb_ctrl_request_t *req) __attribute__((weak));

/**
 * @brief Optional callback for USB CONTROL GET_DESCRIPTOR INTERFACE requests.
 * @param[in] req A reference to a @ref usb_ctrl_request_t.
 */
bool usbd_ctrl_request_get_descriptor_interface_cb(usb_ctrl_request_t *req) __attribute__((weak));

/**
 * @}
 */
