/*
 * usbd-fs-stm32: A lightweight (and very opinionated) USB FS device stack for STM32.
 *
 * SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file usbd.h
 * @brief Main usbd-fs-stm32 header.
 *
 * This header defines the main functions and callbacks implemented by the @c usbd-fs-stm32
 * library.
 */

#pragma once

#include <stdbool.h>

#if defined(STM32F0) || defined(STM32F0xx)
#include <stm32f0xx.h>
#define USB_COUNT0_RX_BLSIZE        (0x1UL << (15U))
#define USB_COUNT1_RX_0_COUNT1_RX_0 (0x000003FFU)
#elif defined(STM32G4) || defined(STM32G4xx)
#include <stm32g4xx.h>
#else
#error "Unsupported STM32 series"
#endif

#ifndef USB
#error "No supported USB device available"
#endif

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
 * Functions to be called by the user when implementing interactions with the host.
 *
 * @{
 */

/**
 * @brief Library initializer.
 *
 * Function that initializes the USB peripheral, including the internal memory
 * buffers.
 *
 * This function must be called during firmware initialization, before entering
 * the firmware main loop.
 */
void usbd_init(void);

/**
 * @brief Library main loop task.
 *
 * Function that runs all the operations related to the USB peripheral.
 *
 * This function must be called periodically from the firmware main loop
 * or from the IRQ handler @c USB_IRQHandler (make sure to initialize the
 * handler properly).
 */
void usbd_task(void);

/**
 * @brief Generate USB string descriptor from internal STM32 serial number.
 * @returns A reference to an internally managed @ref usb_string_description_t.
 *
 * Function that generates a USB string descriptor based on the internal
 * serial number inserted by ST during the manufacturing.
 *
 * It should be called from @ref usbd_get_string_descriptor_cb, when handling
 * the request for a string descriptor with the index @c iSerialNumber, as set
 * in the device descriptor.
 */
const usb_string_descriptor_t* usbd_serial_internal_string_descriptor(void);

/**
 * @brief Transmit data to the host in response to a USB IN request.
 * @param[in] ept    Endpoint number.
 * @param[in] buf    Pointer to a buffer containing data to be transmitted to the host.
 * @param[in] buflen Size of the @c buf buffer, in bytes.
 * @returns A boolean indicating that the data was successfully scheduled for transmission.
 *
 * The buffer should not exceed the size of the endpoint, as described in the
 * endpoint descriptor via @ref usb_endpoint_descriptor_t. To send larger chunks
 * of data, the caller must split the data and call the function multiple times, in
 * response to multiple IN requests.
 *
 * Usually if the final chunk of data sent has the same size of the endpoint buffer,
 * a zero length packet must be also transmitted to the host, to inform it that
 * transmission is complete. This is NOT handled automatically by the library.
 */
bool usbd_in(uint8_t ept, const void *buf, uint16_t buflen);

/**
 * @brief Receive data from the host following a USB OUT request.
 * @param[in]  ept    Endpoint number.
 * @param[out] buf    Pointer to a buffer to receive the data transmitted by the host.
 * @param[in]  buflen Size of the @c buf buffer, in bytes.
 * @returns The number of bytes received from the host.
 *
 * The buffer should not exceed the size of the endpoint, as described in the
 * endpoint descriptor via @ref usb_endpoint_descriptor_t. Ideally, it should be at
 * least the same size of the endpoint.
 *
 * Usually the firmware should continue accepting USB OUT requests and receiving the
 * data by calling this function while the number of bytes received is equal to the
 * endpoint size. When the number of bytes received is smaller than then endpoint
 * size, the reception is completed. This is NOT handled automatically by the library.
 */
uint16_t usbd_out(uint8_t ept, void *buf, uint16_t buflen);

/**
 * @brief Transmit data to the host in response to a CONTROL USB IN request on endpoint 0.
 * @param[in] buf    Pointer to a buffer containing data to be transmitted to the host.
 * @param[in] buflen Size of the @c buf buffer, in bytes.
 * @param[in] reqlen Size of the CONTROL USB IN request data.
 *
 * The buffer may exceed the size of the endpoint 0 (64 bytes), because the function
 * will handle the transmission of the whole buffer automatically.
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
 * Function callbacks that should be implemented by the user to allow the library to
 * interact with the host.
 *
 * @{
 */

/**
 * @brief Required callback to define USB device descriptor.
 * @returns A reference to a constant @ref usb_device_descriptor_t.
 */
const usb_device_descriptor_t* usbd_get_device_descriptor_cb(void);

/**
 * @brief Required allback to define USB configuration descriptor.
 * @returns A reference to a constant @ref usb_config_descriptor_t.
 *
 * @warning The library supports @b only @b one configuration. That is why the
 * callback should not accept any arguments!
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
 * @param[in] idx  The index of the string descriptor to be returned, as defined in the
 *                 descriptor.
 * @returns A reference to a constant @ref usb_string_descriptor_t.
 */
const usb_string_descriptor_t* usbd_get_string_descriptor_cb(uint16_t lang, uint8_t idx);

/**
 * @brief Optional hook callback for USB RESET requests.
 * @param[in] before Notifies if the callback called before or after the device was reset
 *                  (the callback is called in both situations).
 */
void usbd_reset_hook_cb(bool before) __WEAK;

/**
 * @brief Optional hook callback for USB SET_ADDRESS control requests.
 * @param[in] addr The address assigned by the host.
 *
 * Setting the address is the last step of the device enumeration process. The device can
 * be considered enumerated when this hook is called.
 */
void usbd_set_address_hook_cb(uint8_t addr) __WEAK;

/**
 * @brief Optional hook callback for USB SUSPEND requests.
 *
 * This function is responsible to do whatever it needs to do to reduce the power consumption
 * of the device during suspension. The library will enable the internal STM32 low power mode
 * automatically, but that only reduces the consumption of the USB peripheral itself.
 */
void usbd_suspend_hook_cb(void) __WEAK;

/**
 * @brief Optional hook callback for USB RESUME requests.
 *
 * This function is should re-enable whatever was disabled by the @c usbd_suspend_hook_cb when
 * the device entered suspension. The library will dis]able the internal STM32 low power mode
 * automatically.
 */
void usbd_resume_hook_cb(void) __WEAK;

/**
 * @brief Optional callback for USB OUT requests.
 * @param[in] ept Endpoint number.
 */
void usbd_out_cb(uint8_t ept) __WEAK;

/**
 * @brief Optional callback for USB IN requests.
 * @param[in] ept Endpoint number.
 */
void usbd_in_cb(uint8_t ept) __WEAK;

/**
 * @brief Optional callback for USB CONTROL class requests.
 * @param[in] req A reference to a @ref usb_ctrl_request_t.
 */
bool usbd_ctrl_request_handle_class_cb(usb_ctrl_request_t *req) __WEAK;

/**
 * @brief Optional callback for USB CONTROL vendor requests.
 * @param[in] req A reference to a @ref usb_ctrl_request_t.
 */
bool usbd_ctrl_request_handle_vendor_cb(usb_ctrl_request_t *req) __WEAK;

/**
 * @brief Optional callback for USB CONTROL GET_DESCRIPTOR INTERFACE requests.
 * @param[in] req A reference to a @ref usb_ctrl_request_t.
 */
bool usbd_ctrl_request_get_descriptor_interface_cb(usb_ctrl_request_t *req) __WEAK;

/**
 * @}
 */
