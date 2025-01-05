/*
 * usbd-fs-stm32: A lightweight (and very opinionated) USB FS device stack for STM32.
 *
 * SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file usb-std-hid.h
 * @brief Basic USB HID descriptors header.
 *
 * This header defines some macros and types to help define the basic USB
 * HID descriptors.
 */

#pragma once

#include <stdint.h>
#include <usbd.h>

/**
 * @name USB HID descriptor data types
 *
 * Data types to help defining the basic USB HID descriptors.
 *
 * @{
 */

/**
 * @brief USB HID descriptor type.
 */
typedef struct __attribute__((packed)) {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdHID;
    uint8_t bCountryCode;
    uint8_t bNumDescriptors;
    uint8_t bDescriptorType2;
    uint16_t wDescriptorLength;
} usb_hid_descriptor_t;

/**
 * @}
 */

/**
 * @name USB HID descriptor macros
 *
 * Macros to help defining the basic USB HID descriptors.
 *
 * @{
 */

#define USB_REQ_HID_GET_REPORT    0x01
#define USB_REQ_HID_GET_IDLE      0x02
#define USB_REQ_HID_GET_PROTOCOL  0x03
#define USB_REQ_HID_SET_REPORT    0x09
#define USB_REQ_HID_SET_IDLE      0x0a
#define USB_REQ_HID_SET_PROTOCOL  0x0b

#define USB_DESCR_TYPE_HID        0x21
#define USB_DESCR_TYPE_HID_REPORT 0x22
#define USB_DESCR_TYPE_HID_PHYS   0x23

/**
 * @}
 */
