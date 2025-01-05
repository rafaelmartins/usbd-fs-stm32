/*
 * usbd-fs-stm32: A lightweight (and very opinionated) USB FS device stack for STM32.
 *
 * SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file usb-std-midi.h
 * @brief USB basic MIDI-related descriptors header.
 *
 * This header defines some macros and types to help define the basic MIDI-related
 * USB descriptors.
 */

#pragma once

#include <stdint.h>
#include <usbd.h>

/**
 * @name USB MIDI descriptor macros.
 *
 * Macros to help defining the basic MIDI-related USB descriptors.
 *
 * @{
 */

#define USB_MIDI_DESCR_SUBTYPE_MS_HEADER        0x01
#define USB_MIDI_DESCR_SUBTYPE_MS_MIDI_IN_JACK  0x02
#define USB_MIDI_DESCR_SUBTYPE_MS_MIDI_OUT_JACK 0x03
#define USB_MIDI_DESCR_SUBTYPE_MS_ELEMENT       0x04

#define USB_MIDI_DESCR_EPT_SUBTYPE_MS_GENERAL 0x01

#define USB_MIDI_DESCR_JACK_TYPE_MS_EMBEDDED 0x01
#define USB_MIDI_DESCR_JACK_TYPE_MS_EXTERNAL 0x02

/**
 * @}
 */

/**
 * @name USB MIDI descriptor data types.
 *
 * Data typeds to help defining the basic MIDI-related USB descriptors.
 *
 * @{
 */

/**
 * @brief USB MIDI streaming header type.
 */
typedef struct __attribute__((packed)) {
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint16_t bcdMSC;
    uint16_t wTotalLength;
} usb_midi_streaming_header_t;

/**
 * @brief USB MIDI streaming input jack type.
 */
typedef struct __attribute__((packed)) {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bJackType;
    uint8_t bJackID;
    uint8_t iJack;
} usb_midi_streaming_in_jack_t;

/**
 * @brief USB MIDI streaming output jack type.
 */
typedef struct __attribute__((packed)) {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bJackType;
    uint8_t bJackID;
    uint8_t bNrInputPins;
    uint8_t baSourceID;
    uint8_t baSourcePin;
    uint8_t iJack;
} usb_midi_streaming_out_jack_t;

/**
 * @brief USB MIDI streaming endpoint descriptor type.
 */
typedef struct __attribute__((packed)) {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bNumEmbMIDIJack;
    uint8_t baAssocJackID;
} usb_midi_streaming_endpoint_descriptor_t;

/**
 * @}
 */
