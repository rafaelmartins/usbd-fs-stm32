/*
 * usbd-fs-stm32: A lightweight (and very opinionated) USB FS device stack for STM32.
 *
 * SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#pragma once

#include <stdbool.h>

#if defined(STM32F0)
#include <stm32f0xx.h>
#define USB_COUNT0_RX_BLSIZE        (0x1UL << (15U))
#define USB_COUNT1_RX_0_COUNT1_RX_0 (0x000003FFU)
#elif defined(STM32G4)
#include <stm32g4xx.h>
#else
#error "Unsupported STM32 series"
#endif

#ifndef USB
#error "No supported USB device available"
#endif

#include <usb-std.h>

#define USBD_EP0_SIZE 64

void usbd_init(void);
void usbd_task(void);
const usb_string_descriptor_t* usbd_serial_internal_string_descriptor(void);

bool usbd_in(uint8_t ept, const void *buf, uint16_t buflen);
uint16_t usbd_out(uint8_t ept, void *buf, uint16_t buflen);

void usbd_control_in(const void *buf, uint16_t buflen, uint16_t reqlen);


// callbacks

const usb_device_descriptor_t* usbd_get_device_descriptor_cb(void);
const usb_config_descriptor_t* usbd_get_config_descriptor_cb(void);
const usb_interface_descriptor_t* usbd_get_interface_descriptor_cb(uint16_t itf);
const usb_string_descriptor_t* usbd_get_string_descriptor_cb(uint16_t lang, uint8_t idx);

void usbd_reset_hook_cb(bool before) __WEAK;
void usbd_set_address_hook_cb(uint8_t addr) __WEAK;
void usbd_suspend_hook_cb(void) __WEAK;
void usbd_resume_hook_cb(void) __WEAK;

void usbd_out_cb(uint8_t ept) __WEAK;
void usbd_in_cb(uint8_t ept) __WEAK;

bool usbd_ctrl_request_handle_class_cb(usb_ctrl_request_t *req) __WEAK;
bool usbd_ctrl_request_handle_vendor_cb(usb_ctrl_request_t *req) __WEAK;
bool usbd_ctrl_request_get_descriptor_interface_cb(usb_ctrl_request_t *req) __WEAK;
