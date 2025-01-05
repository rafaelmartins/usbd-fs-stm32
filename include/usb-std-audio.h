/*
 * usbd-fs-stm32: A lightweight (and very opinionated) USB FS device stack for STM32.
 *
 * SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file usb-std-audio.h
 * @brief USB basic audio-related descriptors header.
 *
 * This header defines some macros and types to help define the basic audio-related
 * USB descriptors.
 */

#pragma once

#include <stdint.h>
#include <usbd.h>

/**
 * @name USB audio descriptor macros
 *
 * Macros to help defining the basic audio-related USB descriptors.
 *
 * @{
 */

#define USB_AUDIO_DESCR_UAC_VERSION_1 0x00
#define USB_AUDIO_DESCR_UAC_VERSION_2 0x20
#define USB_AUDIO_DESCR_UAC_VERSION_3 0x30

#define USB_AUDIO_DESCR_DEV_SUBCLASS_AUDIOCONTROL   0x01
#define USB_AUDIO_DESCR_DEV_SUBCLASS_AUDIOSTREAMING 0x02
#define USB_AUDIO_DESCR_DEV_SUBCLASS_MIDISTREAMING  0x03

#define USB_AUDIO_DESCR_SUBTYPE_UAC_HEADER           0x01
#define USB_AUDIO_DESCR_SUBTYPE_UAC_INPUT_TERMINAL   0x02
#define USB_AUDIO_DESCR_SUBTYPE_UAC_OUTPUT_TERMINAL  0x03
#define USB_AUDIO_DESCR_SUBTYPE_UAC_MIXER_UNIT       0x04
#define USB_AUDIO_DESCR_SUBTYPE_UAC_SELECTOR_UNIT0   0x05
#define USB_AUDIO_DESCR_SUBTYPE_UAC_FEATURE_UNIT     0x06
#define USB_AUDIO_DESCR_SUBTYPE_UAC1_PROCESSING_UNIT 0x07
#define USB_AUDIO_DESCR_SUBTYPE_UAC1_EXTENSION_UNIT  0x08

/**
 * @}
 */

/**
 * @name USB audio descriptor data types
 *
 * Data types to help defining the basic audio-related USB descriptors.
 *
 * @{
 */

/**
 * @brief USB audio endpoint descriptor type.
 */
typedef struct __attribute__((packed)) {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bEndpointAddress;
    uint8_t  bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t  bInterval;
    uint8_t  bRefresh;
    uint8_t  bSynchAddress;
} usb_audio_endpoint_descriptor_t;

/**
 * @brief USB audio control header type.
 */
typedef struct __attribute__((packed)) {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint16_t bcdADC;
    uint16_t wTotalLength;
    uint8_t  bInCollection;
    uint8_t  baInterfaceNr;
} usb_audio_ctrl_header_t;

/**
 * @}
 */
