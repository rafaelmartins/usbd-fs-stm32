/*
 * usbd-fs-stm32: A lightweight (and very opinionated) USB FS device stack for STM32.
 *
 * SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file usb-std.h
 * @brief USB basic standard descriptors header.
 *
 * This header defines some macros and types to help define the basic standard
 * USB descriptors.
 */

#pragma once

#include <stdint.h>
#include <usbd.h>

/**
 * @name USB control request (setup) data macros.
 *
 * Macros to help with bitwise comparision of SETUP request packets.
 *
 * @{
 */

#define USB_REQ_DIR_HOST_TO_DEVICE (0 << 7)
#define USB_REQ_DIR_DEVICE_TO_HOST (1 << 7)
#define USB_REQ_DIR_MASK           (1 << 7)

#define USB_REQ_TYPE_STANDARD (0 << 5)
#define USB_REQ_TYPE_CLASS    (1 << 5)
#define USB_REQ_TYPE_VENDOR   (2 << 5)
#define USB_REQ_TYPE_MASK     (3 << 5)

#define USB_REQ_RCPT_DEVICE    (0 << 0)
#define USB_REQ_RCPT_INTERFACE (1 << 0)
#define USB_REQ_RCPT_ENDPOINT  (2 << 0)
#define USB_REQ_RCPT_OTHER     (3 << 0)
#define USB_REQ_RCPT_MASK      (3 << 0)

#define USB_REQ_GET_STATUS        0x00
#define USB_REQ_CLEAR_FEATURE     0x01
#define USB_REQ_SET_FEATURE       0x03
#define USB_REQ_SET_ADDRESS       0x05
#define USB_REQ_GET_DESCRIPTOR    0x06
#define USB_REQ_SET_DESCRIPTOR    0x07
#define USB_REQ_GET_CONFIGURATION 0x08
#define USB_REQ_SET_CONFIGURATION 0x09
#define USB_REQ_GET_INTERFACE     0x0a
#define USB_REQ_SET_INTERFACE     0x0b
#define USB_REQ_SYNCH_FRAME       0x0c

/**
 * @}
 */


/**
 * @name USB descriptor macros.
 *
 * Macros to help defining the basic standard USB descriptors.
 *
 * @{
 */

#define USB_DESCR_TYPE_DEVICE                    0x01
#define USB_DESCR_TYPE_CONFIGURATION             0x02
#define USB_DESCR_TYPE_STRING                    0x03
#define USB_DESCR_TYPE_INTERFACE                 0x04
#define USB_DESCR_TYPE_ENDPOINT                  0x05
#define USB_DESCR_TYPE_DEVICE_QUALIFIER          0x06
#define USB_DESCR_TYPE_OTHER_SPEED_CONFIGURATION 0x07

#define USB_DESCR_CONFIG_ATTR_RESERVED      (1 << 7)
#define USB_DESCR_CONFIG_ATTR_SELF_POWERED  (1 << 6)
#define USB_DESCR_CONFIG_ATTR_REMOTE_WAKEUP (1 << 5)

#define USB_DESCR_DEV_CLASS_PER_INTERFACE       0x00
#define USB_DESCR_DEV_CLASS_AUDIO               0x01
#define USB_DESCR_DEV_CLASS_COMM                0x02
#define USB_DESCR_DEV_CLASS_HID                 0x03
#define USB_DESCR_DEV_CLASS_PHYSICAL            0x05
#define USB_DESCR_DEV_CLASS_STILL_IMAGE         0x06
#define USB_DESCR_DEV_CLASS_PRINTER             0x07
#define USB_DESCR_DEV_CLASS_MASS_STORAGE        0x08
#define USB_DESCR_DEV_CLASS_HUB                 0x09
#define USB_DESCR_DEV_CLASS_CDC_DATA            0x0a
#define USB_DESCR_DEV_CLASS_CSCID               0x0b
#define USB_DESCR_DEV_CLASS_CONTENT_SEC         0x0d
#define USB_DESCR_DEV_CLASS_VIDEO               0x0e
#define USB_DESCR_DEV_CLASS_WIRELESS_CONTROLLER 0xe0
#define USB_DESCR_DEV_CLASS_PERSONAL_HEALTHCARE 0x0f
#define USB_DESCR_DEV_CLASS_AUDIO_VIDEO         0x10
#define USB_DESCR_DEV_CLASS_BILLBOARD           0x11
#define USB_DESCR_DEV_CLASS_USB_TYPE_C_BRIDGE   0x12
#define USB_DESCR_DEV_CLASS_MISC                0xef
#define USB_DESCR_DEV_CLASS_APP_SPEC            0xfe
#define USB_DESCR_DEV_CLASS_VENDOR_SPEC         0xff
#define USB_DESCR_DEV_SUBCLASS_VENDOR_SPEC      0xff

#define USB_DESCR_EPT_ADDR_DIR_OUT  (0 << 7)
#define USB_DESCR_EPT_ADDR_DIR_IN   (1 << 7)
#define USB_DESCR_EPT_ADDR_DIR_MASK (1 << 7)

#define USB_DESCR_EPT_ATTR_CONTROL                (0 << 0)
#define USB_DESCR_EPT_ATTR_ISOCHRONOUS            (1 << 0)
#define USB_DESCR_EPT_ATTR_BULK                   (2 << 0)
#define USB_DESCR_EPT_ATTR_INTERRUPT              (3 << 0)
#define USB_DESCR_EPT_ATTR_NO_SYNC                (0 << 2)
#define USB_DESCR_EPT_ATTR_ASYNC                  (1 << 2)
#define USB_DESCR_EPT_ATTR_ADAPTIVE               (2 << 2)
#define USB_DESCR_EPT_ATTR_SYNC                   (3 << 2)
#define USB_DESCR_EPT_ATTR_DATA                   (0 << 4)
#define USB_DESCR_EPT_ATTR_FEEDBACK               (1 << 2)
#define USB_DESCR_EPT_ATTR_IMPLICIT_FEEDBACK_DATA (2 << 2)

#define USB_DESCR_FEAT_ENDPOINT_HALT        0x00
#define USB_DESCR_FEAT_DEVICE_REMOTE_WAKEUP 0x01
#define USB_DESCR_FEAT_TEST_MODE            0x02

/**
 * @}
 */

/**
 * @brief USB control request (setup) type.
 */
typedef __PACKED_STRUCT {
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} usb_ctrl_request_t;

/**
 * @brief USB device descriptor type.
 */
typedef __PACKED_STRUCT {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t iManufacturer;
    uint8_t iProduct;
    uint8_t iSerialNumber;
    uint8_t bNumConfigurations;
} usb_device_descriptor_t;

/**
 * @brief USB configuration descriptor type.
 */
typedef __PACKED_STRUCT {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wTotalLength;
    uint8_t bNumInterfaces;
    uint8_t bConfigurationValue;
    uint8_t iConfiguration;
    uint8_t bmAttributes;
    uint8_t bMaxPower;
} usb_config_descriptor_t;

/**
 * @brief USB string descriptor type.
 */
typedef __PACKED_STRUCT {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wData[];
} usb_string_descriptor_t;

/**
 * @brief USB interface descriptor type.
 */
typedef __PACKED_STRUCT {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;
} usb_interface_descriptor_t;

/**
 * @brief USB endpoint descriptor type.
 */
typedef __PACKED_STRUCT {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bEndpointAddress;
    uint8_t bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t bInterval;
} usb_endpoint_descriptor_t;
