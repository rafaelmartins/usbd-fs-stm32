/*
 * usbd-fs-stm32: A lightweight (and very opinionated) USB FS device stack for STM32.
 *
 * SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <stdint.h>
#include <usbd.h>


// Descriptor macros

#define USB_MIDI_DESCR_SUBTYPE_MS_HEADER        0x01
#define USB_MIDI_DESCR_SUBTYPE_MS_MIDI_IN_JACK  0x02
#define USB_MIDI_DESCR_SUBTYPE_MS_MIDI_OUT_JACK 0x03
#define USB_MIDI_DESCR_SUBTYPE_MS_ELEMENT       0x04

#define USB_MIDI_DESCR_EPT_SUBTYPE_MS_GENERAL 0x01

#define USB_MIDI_DESCR_JACK_TYPE_MS_EMBEDDED 0x01
#define USB_MIDI_DESCR_JACK_TYPE_MS_EXTERNAL 0x02


// Descriptor types

typedef __PACKED_STRUCT {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint16_t bcdMSC;
    uint16_t wTotalLength;
} usb_midi_streaming_header_t;

typedef __PACKED_STRUCT {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bJackType;
    uint8_t bJackID;
    uint8_t iJack;
} usb_midi_streaming_in_jack_t;

typedef __PACKED_STRUCT {
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

typedef __PACKED_STRUCT {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bDescriptorSubtype;
    uint8_t bNumEmbMIDIJack;
    uint8_t baAssocJackID;
} usb_midi_streaming_endpoint_descriptor_t;
