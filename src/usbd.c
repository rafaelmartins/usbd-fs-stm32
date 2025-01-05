/*
 * usbd-fs-stm32: A lightweight (and very opinionated) USB FS device stack for STM32.
 *
 * SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <usbd.h>

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

#ifndef USBD_EP1_IN_SIZE
#define USBD_EP1_IN_SIZE 0
#endif
#ifndef USBD_EP1_OUT_SIZE
#define USBD_EP1_OUT_SIZE 0
#endif
#ifndef USBD_EP2_IN_SIZE
#define USBD_EP2_IN_SIZE 0
#endif
#ifndef USBD_EP2_OUT_SIZE
#define USBD_EP2_OUT_SIZE 0
#endif
#ifndef USBD_EP3_IN_SIZE
#define USBD_EP3_IN_SIZE 0
#endif
#ifndef USBD_EP3_OUT_SIZE
#define USBD_EP3_OUT_SIZE 0
#endif
#ifndef USBD_EP4_IN_SIZE
#define USBD_EP4_IN_SIZE 0
#endif
#ifndef USBD_EP4_OUT_SIZE
#define USBD_EP4_OUT_SIZE 0
#endif
#ifndef USBD_EP5_IN_SIZE
#define USBD_EP5_IN_SIZE 0
#endif
#ifndef USBD_EP5_OUT_SIZE
#define USBD_EP5_OUT_SIZE 0
#endif
#ifndef USBD_EP6_IN_SIZE
#define USBD_EP6_IN_SIZE 0
#endif
#ifndef USBD_EP6_OUT_SIZE
#define USBD_EP6_OUT_SIZE 0
#endif
#ifndef USBD_EP7_IN_SIZE
#define USBD_EP7_IN_SIZE 0
#endif
#ifndef USBD_EP7_OUT_SIZE
#define USBD_EP7_OUT_SIZE 0
#endif

#ifndef USBD_EP1_TYPE
#define USBD_EP1_TYPE BULK
#endif
#ifndef USBD_EP2_TYPE
#define USBD_EP2_TYPE BULK
#endif
#ifndef USBD_EP3_TYPE
#define USBD_EP3_TYPE BULK
#endif
#ifndef USBD_EP4_TYPE
#define USBD_EP4_TYPE BULK
#endif
#ifndef USBD_EP5_TYPE
#define USBD_EP5_TYPE BULK
#endif
#ifndef USBD_EP6_TYPE
#define USBD_EP6_TYPE BULK
#endif
#ifndef USBD_EP7_TYPE
#define USBD_EP7_TYPE BULK
#endif

#if (USBD_EP1_IN_SIZE + USBD_EP1_OUT_SIZE + USBD_EP2_IN_SIZE + USBD_EP2_OUT_SIZE + \
     USBD_EP3_IN_SIZE + USBD_EP3_OUT_SIZE + USBD_EP4_IN_SIZE + USBD_EP4_OUT_SIZE + \
     USBD_EP5_IN_SIZE + USBD_EP5_OUT_SIZE + USBD_EP6_IN_SIZE + USBD_EP6_OUT_SIZE + \
     USBD_EP7_IN_SIZE + USBD_EP7_OUT_SIZE) > (1024 - 64 - USBD_EP0_SIZE - USBD_EP0_SIZE)
#error "Unsupported endpoint configuration, not enough USB SRAM available"
#endif

typedef struct {
    __IOM uint16_t addr;
    __IOM uint16_t cnt;
} __ALIGNED(2) pma_entry_t;

static struct {
    uint16_t type;
    __IOM uint16_t* reg;
    __IOM pma_entry_t* pma_in;
    __IOM pma_entry_t* pma_out;
    uint8_t size_in;
    uint8_t size_out;
} endpoints[] = {
    {
        .type     = USB_EP_CONTROL,
        .reg      = (__IOM uint16_t*) &(USB->EP0R),
        .pma_in   = (__IOM pma_entry_t*) USB_PMAADDR,
        .pma_out  = (__IOM pma_entry_t*) (USB_PMAADDR + sizeof(pma_entry_t)),
        .size_in  = USBD_EP0_SIZE,
        .size_out = USBD_EP0_SIZE,
    },

#define __endpoint(EPT, TYP)                                                               \
    {                                                                                      \
        .type     = USB_EP_ ## TYP,                                                        \
        .reg      = (__IOM uint16_t*) &(USB->EP ## EPT ## R),                              \
        .pma_in   = (__IOM pma_entry_t*) (USB_PMAADDR + (EPT << 3)),                       \
        .pma_out  = (__IOM pma_entry_t*) (USB_PMAADDR + (EPT << 3) + sizeof(pma_entry_t)), \
        .size_in  = USBD_EP ## EPT ## _IN_SIZE,                                            \
        .size_out = USBD_EP ## EPT ## _OUT_SIZE,                                           \
    }
#define _endpoint(EPT, TYP) __endpoint(EPT, TYP)
#define endpoint(EPT)       _endpoint(EPT, USBD_EP ## EPT ## _TYPE)

    endpoint(1),
    endpoint(2),
    endpoint(3),
    endpoint(4),
    endpoint(5),
    endpoint(6),
    endpoint(7),

#undef endpoint
#undef _endpoint
#undef __endpoint
};


static void
pma_init(void)
{
    uint32_t entry_addr = USB_PMAADDR;
    uint32_t mem_addr = USB_PMAADDR + 16 * sizeof(pma_entry_t);  // right after entry table

    for (uint8_t i = 0; i < 8; i++) {
        pma_entry_t *e = (pma_entry_t*) entry_addr;
        uint8_t *m = (uint8_t*) mem_addr;

        e->addr = m - ((uint8_t*) USB_PMAADDR);
        e->cnt = 0;

        entry_addr += sizeof(pma_entry_t);
        mem_addr += endpoints[i].size_in;

        e = (pma_entry_t*) entry_addr;
        m = (uint8_t*) mem_addr;

        e->addr = m - ((uint8_t*) USB_PMAADDR);
        if (endpoints[i].size_out > 62)
            e->cnt = USB_COUNT0_RX_BLSIZE | ((endpoints[i].size_out >> 6) & 0b11111);
        else
            e->cnt = ((endpoints[i].size_out >> 1) & 0b11111);

        entry_addr += sizeof(pma_entry_t);
        mem_addr += endpoints[i].size_out;
    }

    USB->BTABLE = 0;
}


bool
usbd_in(uint8_t ept, const void *buf, uint16_t buflen)
{
    if (ept >= 8)
        return false;

    __IO pma_entry_t *e = endpoints[ept].pma_in;
    if (e->addr == 0)
        return false;

    const uint8_t *src = buf;
    __IO uint16_t *dst = (uint16_t*) (USB_PMAADDR + e->addr);

    uint16_t tmp;
    for (uint16_t i = 0; i < ((buflen + 1) >> 1); i++) {
        tmp = *(src++);
        tmp |= (((uint16_t) *(src++)) << 8);
        *(dst++) = tmp;
    }
    e->cnt = buflen;

    __IO uint16_t *ep = endpoints[ept].reg;
    *ep = (*ep ^ USB_EP_TX_VALID) & (USB_EPREG_MASK | USB_EPTX_STAT);
    return true;
}

uint16_t
usbd_out(uint8_t ept, void *buf, uint16_t buflen)
{
    if (ept >= 8)
        return false;

    __IO pma_entry_t *e = endpoints[ept].pma_out;
    if (e->addr == 0)
        return 0;

    uint16_t rv = e->cnt & USB_COUNT1_RX_0_COUNT1_RX_0;
    rv = (rv > buflen) ? buflen : rv;
    memcpy(buf, (void*) (USB_PMAADDR + e->addr), rv);

    __IO uint16_t *ep = endpoints[ept].reg;
    *ep = (*ep ^ USB_EP_RX_VALID) & (USB_EPREG_MASK | USB_EPRX_STAT);
    return rv;
}


static const uint8_t* ctrl_in_buf = NULL;
static uint16_t ctrl_in_buflen = 0;

void
usbd_control_in(const void *buf, uint16_t buflen, uint16_t reqlen)
{
    uint16_t total = reqlen < buflen ? reqlen : buflen;
    uint16_t l = total > USBD_EP0_SIZE ? USBD_EP0_SIZE : total;
    usbd_in(0, (uint8_t*) buf, l);
    ctrl_in_buf = total > USBD_EP0_SIZE ? buf + USBD_EP0_SIZE : NULL;
    ctrl_in_buflen = total > USBD_EP0_SIZE ? total - USBD_EP0_SIZE : 0;
}

static bool
usbd_control_in_resume(void)
{
    if (ctrl_in_buf == NULL)
        return false;

    uint16_t l = ctrl_in_buflen > USBD_EP0_SIZE ? USBD_EP0_SIZE : ctrl_in_buflen;
    usbd_in(0, ctrl_in_buf, l);
    ctrl_in_buf = ctrl_in_buflen > USBD_EP0_SIZE ? ctrl_in_buf + USBD_EP0_SIZE : NULL;
    ctrl_in_buflen = ctrl_in_buflen > USBD_EP0_SIZE ? ctrl_in_buflen - USBD_EP0_SIZE : 0;
    return true;
}


__STATIC_FORCEINLINE uint8_t
get_config_bConfigurationValue(void)
{
    const usb_config_descriptor_t *cfg = usbd_get_config_descriptor_cb();
    if (cfg == NULL)
        return 0;
    return cfg->bConfigurationValue;
}

__STATIC_FORCEINLINE bool
write_device_descriptor(usb_ctrl_request_t *req)
{
    const usb_device_descriptor_t *dev = usbd_get_device_descriptor_cb();
    if (dev == NULL)
        return false;

    usbd_control_in(dev, dev->bLength, req->wLength);
    return true;
}

__STATIC_FORCEINLINE bool
write_config_descriptor(usb_ctrl_request_t *req)
{
    const usb_config_descriptor_t *cfg = usbd_get_config_descriptor_cb();
    if (cfg == NULL)
        return false;

    usbd_control_in(cfg, cfg->wTotalLength, req->wLength);
    return true;
}

__STATIC_FORCEINLINE bool
write_string_descriptor(usb_ctrl_request_t *req)
{
    const usb_string_descriptor_t *str = usbd_get_string_descriptor_cb(req->wIndex, req->wValue);
    if (str == NULL)
        return false;

    usbd_control_in(str, str->bLength, req->wLength);
    return true;
}


static enum {
    STATE_DEFAULT,
    STATE_ADDRESS,
    STATE_CONFIGURED,
} state = STATE_DEFAULT;

static bool set_address = false;
static uint16_t address = 0;

static bool
handle_ctrl_setup(usb_ctrl_request_t *req)
{
    if ((req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_CLASS) {
        if (usbd_ctrl_request_handle_class_cb)
            return usbd_ctrl_request_handle_class_cb(req);
        return false;
    }

    if ((req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_VENDOR) {
        if (usbd_ctrl_request_handle_vendor_cb)
            return usbd_ctrl_request_handle_vendor_cb(req);
        return false;
    }

    switch (req->bRequest) {
    case USB_REQ_GET_STATUS:
        if (((req->bmRequestType & USB_REQ_DIR_MASK) == USB_REQ_DIR_HOST_TO_DEVICE) ||
            (state != STATE_CONFIGURED))
            break;

        uint8_t status[2] = {0, 0};

        switch (req->bmRequestType & USB_REQ_RCPT_MASK) {
        case USB_REQ_RCPT_DEVICE:
            {
                const usb_config_descriptor_t *cfg = usbd_get_config_descriptor_cb();
                if (cfg != NULL && (cfg->bmAttributes & USB_DESCR_CONFIG_ATTR_SELF_POWERED))
                    status[0] |= (1 << 0);
            }
            break;

        case USB_REQ_RCPT_INTERFACE:
            if (usbd_get_interface_descriptor_cb(req->wIndex) == NULL)
                return false;
            break;

        case USB_REQ_RCPT_ENDPOINT:
            {
                uint8_t ept = req->wIndex & 0x7;
                if (req->wIndex & USB_DESCR_EPT_ADDR_DIR_IN) {
                    if (endpoints[ept].size_in == 0)
                        return false;
                    if ((*(endpoints[ept].reg) & USB_EPTX_STAT) == USB_EP_TX_STALL)
                        status[0] |= (1 << 0);
                }
                else {
                    if (endpoints[ept].size_out == 0)
                        return false;
                    if ((*(endpoints[ept].reg) & USB_EPRX_STAT) == USB_EP_RX_STALL)
                        status[0] |= (1 << 0);
                }
            }
            break;
        }

        usbd_control_in(status, sizeof(status), req->wLength);
        return true;

    case USB_REQ_CLEAR_FEATURE:
        {
            if (((req->bmRequestType & USB_REQ_DIR_MASK) == USB_REQ_DIR_DEVICE_TO_HOST) ||
                ((req->bmRequestType & USB_REQ_RCPT_MASK) != USB_REQ_RCPT_ENDPOINT) ||
                (req->wValue != USB_FEAT_ENDPOINT_HALT) ||
                (state != STATE_CONFIGURED))
                break;

            uint8_t ept = req->wIndex & 0x7;
            if ((endpoints[ept].type != USB_EP_BULK) && (endpoints[ept].type != USB_EP_INTERRUPT))
                break;

            if (req->wIndex & USB_DESCR_EPT_ADDR_DIR_IN) {
                if (endpoints[ept].size_in != 0) {
                    *(endpoints[ept].reg) = (*(endpoints[ept].reg) ^ USB_EP_TX_NAK) &
                        (USB_EPREG_MASK | USB_EPTX_STAT | USB_EP_DTOG_TX);
                    return true;
                }
            }
            else if (endpoints[ept].size_out != 0) {
                *(endpoints[ept].reg) = (*(endpoints[ept].reg) ^ USB_EP_RX_VALID) &
                    (USB_EPREG_MASK | USB_EPRX_STAT | USB_EP_DTOG_RX);
                return true;
            }
        }
        break;

    case USB_REQ_SET_FEATURE:
        {
            if (((req->bmRequestType & USB_REQ_DIR_MASK) == USB_REQ_DIR_DEVICE_TO_HOST) ||
                ((req->bmRequestType & USB_REQ_RCPT_MASK) != USB_REQ_RCPT_ENDPOINT) ||
                (req->wValue != USB_FEAT_ENDPOINT_HALT) ||
                (state != STATE_CONFIGURED))
                break;

            uint8_t ept = req->wIndex & 0x7;
            if ((endpoints[ept].type != USB_EP_BULK) && (endpoints[ept].type != USB_EP_INTERRUPT))
                break;

            if (req->wIndex & USB_DESCR_EPT_ADDR_DIR_IN) {
                if (endpoints[ept].size_in != 0) {
                    *(endpoints[ept].reg) = (*(endpoints[ept].reg) ^ USB_EP_TX_STALL) &
                        (USB_EPREG_MASK | USB_EPTX_STAT);
                    return true;
                }
            }
            else if (endpoints[ept].size_out != 0) {
                *(endpoints[ept].reg) = (*(endpoints[ept].reg) ^ USB_EP_RX_STALL) &
                    (USB_EPREG_MASK | USB_EPRX_STAT);
                return true;
            }
        }
        break;

    case USB_REQ_SET_ADDRESS:
        if (((req->bmRequestType & USB_REQ_DIR_MASK) == USB_REQ_DIR_DEVICE_TO_HOST) ||
            ((req->bmRequestType & USB_REQ_RCPT_MASK) != USB_REQ_RCPT_DEVICE))
            break;

        switch (state) {
        case STATE_DEFAULT:
            if (req->wValue == 0)
                break;
            // fall through

        case STATE_ADDRESS:
            address = (req->wValue & USB_DADDR_ADD);
            set_address = true;
            if (usbd_set_address_hook_cb)
                usbd_set_address_hook_cb(address);
            break;

        case STATE_CONFIGURED:
            break;
        }

        return true;

    case USB_REQ_GET_DESCRIPTOR:
        if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_REQ_DIR_HOST_TO_DEVICE)
            break;

        switch (req->bmRequestType & USB_REQ_RCPT_MASK) {
        case USB_REQ_RCPT_DEVICE:
            switch (req->wValue >> 8) {
            case USB_DESCR_TYPE_DEVICE:
                return write_device_descriptor(req);

            case USB_DESCR_TYPE_CONFIGURATION:
                return write_config_descriptor(req);

            case USB_DESCR_TYPE_STRING:
                return write_string_descriptor(req);
            }
            break;

        case USB_REQ_RCPT_INTERFACE:
            if (usbd_ctrl_request_get_descriptor_interface_cb)
                return usbd_ctrl_request_get_descriptor_interface_cb(req);
            break;
        }
        break;

    case USB_REQ_SET_DESCRIPTOR:
        // not supported
        break;

    case USB_REQ_GET_CONFIGURATION:
        if (((req->bmRequestType & USB_REQ_DIR_MASK) == USB_REQ_DIR_HOST_TO_DEVICE) ||
            ((req->bmRequestType & USB_REQ_RCPT_MASK) != USB_REQ_RCPT_DEVICE))
            break;

        uint8_t config = state == STATE_CONFIGURED ? get_config_bConfigurationValue() : 0;
        usbd_control_in(&config, sizeof(config), req->wLength);
        return true;

    case USB_REQ_SET_CONFIGURATION:
        if (((req->bmRequestType & USB_REQ_DIR_MASK) == USB_REQ_DIR_DEVICE_TO_HOST) ||
            ((req->bmRequestType & USB_REQ_RCPT_MASK) != USB_REQ_RCPT_DEVICE) ||
            (state == STATE_DEFAULT))
            break;

        if (req->wValue == 0) {
            state = STATE_ADDRESS;
            for (uint8_t i = 1; i < 8; i++)
                *(endpoints[i].reg) &= ~USB_EPREG_MASK;
        }
        else if (((uint8_t) req->wValue) == get_config_bConfigurationValue()) {
            state = STATE_CONFIGURED;

            for (uint8_t i = 1; i < 8; i++) {
                if (endpoints[i].size_in == 0 && endpoints[i].size_out == 0)
                    continue;

                __IO uint16_t *ep = endpoints[i].reg;
                *ep &= ~USB_EPREG_MASK;
                *ep |= endpoints[i].type | i;

                if (endpoints[i].size_in != 0)
                    *ep = (*ep ^ USB_EP_TX_NAK) &
                        (USB_EPREG_MASK | USB_EPTX_STAT | USB_EP_DTOG_TX);
                if (endpoints[i].size_out != 0)
                    *ep = (*ep ^ USB_EP_RX_VALID) &
                        (USB_EPREG_MASK | USB_EPRX_STAT | USB_EP_DTOG_RX);
            }
        }
        else
            break;

        return true;

    case USB_REQ_GET_INTERFACE:
        {
            if (((req->bmRequestType & USB_REQ_DIR_MASK) == USB_REQ_DIR_HOST_TO_DEVICE) ||
                ((req->bmRequestType & USB_REQ_RCPT_MASK) != USB_REQ_RCPT_INTERFACE) ||
                (state != STATE_CONFIGURED))
                break;

            const usb_interface_descriptor_t *itf = usbd_get_interface_descriptor_cb(req->wIndex);
            if (itf == NULL)
                break;

            usbd_control_in(&(itf->bAlternateSetting), sizeof(itf->bAlternateSetting), req->wLength);
            return true;
        }
        break;

    case USB_REQ_SET_INTERFACE:
        {
            if (((req->bmRequestType & USB_REQ_DIR_MASK) == USB_REQ_DIR_DEVICE_TO_HOST) ||
                ((req->bmRequestType & USB_REQ_RCPT_MASK) != USB_REQ_RCPT_INTERFACE) ||
                (state != STATE_CONFIGURED))
                break;

            // no alternate setting supported, but someone may still try to re-set
            const usb_interface_descriptor_t *itf = usbd_get_interface_descriptor_cb(req->wIndex);
            if (itf == NULL)
                break;

            if (itf->bAlternateSetting == (uint8_t) req->wValue)
                return true;
        }
        break;

    case USB_REQ_SYNCH_FRAME:
        // isochronous endpoints not supported
        break;
    }

    return false;
}


void
usbd_init(void)
{
#ifdef STM32F0
    RCC->APB1ENR |= RCC_APB1ENR_USBEN;
    RCC->APB1RSTR |= RCC_APB1RSTR_USBRST;
    RCC->APB1RSTR &= ~RCC_APB1RSTR_USBRST;
#endif

#ifdef STM32G4
    RCC->APB1ENR1 |= RCC_APB1ENR1_USBEN;
    RCC->APB1RSTR1 |= RCC_APB1RSTR1_USBRST;
    RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_USBRST;
#endif

    USB->CNTR &= ~USB_CNTR_PDWN;

    pma_init();

    USB->ISTR = 0;
    USB->CNTR = USB_CNTR_CTRM | USB_CNTR_WKUPM | USB_CNTR_SUSPM | USB_CNTR_RESETM;
    if (usbd_in_cb)
        USB->CNTR |= USB_CNTR_SOFM;
    USB->BCDR = USB_BCDR_DPPU;
}


void
usbd_task(void)
{
    uint16_t istr = USB->ISTR & (USB_ISTR_CTR | USB_ISTR_WKUP | USB_ISTR_SUSP | USB_ISTR_RESET | USB_ISTR_SOF);
    if (istr == 0)
        return;

    if (istr & USB_ISTR_WKUP) {
        USB->ISTR &= ~(USB_ISTR_SUSP | USB_ISTR_WKUP);
        USB->CNTR &= ~USB_CNTR_FSUSP;
        if (usbd_resume_hook_cb)
            usbd_resume_hook_cb();
        return;
    }

    if (istr & USB_ISTR_SUSP) {
        USB->ISTR &= ~USB_ISTR_SUSP;
        USB->CNTR |= USB_CNTR_FSUSP;
        if (usbd_suspend_hook_cb)
            usbd_suspend_hook_cb();
        return;
    }

    if (istr & USB_ISTR_RESET) {
        USB->ISTR &= ~USB_ISTR_RESET;

        if (usbd_reset_hook_cb)
            usbd_reset_hook_cb(true);

        for (uint8_t i = 0; i < 8; i++)
            *(endpoints[i].reg) &= ~USB_EPREG_MASK;

        state = STATE_DEFAULT;
        address = 0;
        USB->DADDR = USB_DADDR_EF | address;

        USB->EP0R |= endpoints[0].type;
        USB->EP0R = (USB->EP0R ^ (USB_EP_RX_VALID | USB_EP_TX_NAK)) &
            (USB_EPREG_MASK | USB_EPRX_STAT | USB_EP_DTOG_RX | USB_EP_DTOG_TX);

        if (usbd_reset_hook_cb)
            usbd_reset_hook_cb(false);
        return;
    }

    static uint8_t current_ep = 1;
    if (usbd_in_cb && (istr & USB_ISTR_SOF)) {
        USB->ISTR &= ~USB_ISTR_SOF;

        uint8_t ep = current_ep++;
        if (current_ep >= 8)
            current_ep = 1;

        if ((endpoints[ep].size_in != 0) &&
            (((*endpoints[ep].reg) & (USB_EPTX_STAT | USB_EPADDR_FIELD)) == (USB_EP_TX_NAK | ep))) {
            usbd_in_cb(ep);
            return;
        }
    }

    if (istr & USB_ISTR_CTR) {
        uint8_t ep = USB->ISTR & USB_ISTR_EP_ID;

        if (ep == 0) {
            if (USB->EP0R & (USB_EP_CTR_RX|USB_EP_SETUP)) {
                USB->EP0R &= USB_EPREG_MASK ^ USB_EP_CTR_RX;

                usb_ctrl_request_t req;
                uint16_t len = usbd_out(0, &req, sizeof(usb_ctrl_request_t));
                if ((len == sizeof(usb_ctrl_request_t)) && handle_ctrl_setup(&req)) {
                    if ((req.bmRequestType & USB_REQ_DIR_MASK) == USB_REQ_DIR_HOST_TO_DEVICE)
                        usbd_control_in(NULL, 0, req.wLength);
                    return;
                }

                USB->EP0R = (USB->EP0R ^ USB_EP_TX_STALL) & (USB_EPREG_MASK | USB_EPTX_STAT);
                USB->EP0R = (USB->EP0R ^ USB_EP_RX_STALL) & (USB_EPREG_MASK | USB_EPRX_STAT);
                return;
            }

            if (USB->EP0R & USB_EP_CTR_TX) {
                USB->EP0R &= USB_EPREG_MASK ^ USB_EP_CTR_TX;

                if (set_address) {
                    USB->DADDR = USB_DADDR_EF | address;
                    set_address = false;
                    state = STATE_ADDRESS;
                }

                if (usbd_control_in_resume())
                    return;
            }
        }

        if (*(endpoints[ep].reg) & USB_EP_CTR_RX) {
            *(endpoints[ep].reg) &= USB_EPREG_MASK ^ USB_EP_CTR_RX;
            if (usbd_out_cb)
                usbd_out_cb(ep);
        }
        if (*(endpoints[ep].reg) & USB_EP_CTR_TX)
            *(endpoints[ep].reg) &= USB_EPREG_MASK ^ USB_EP_CTR_TX;
    }
}


static inline uint8_t
to_hex(uint8_t v)
{
    if ((v & 0xf) > 9)
        return (v & 0xf) - 10 + 'A';
    return (v & 0xf) + '0';
}

const usb_string_descriptor_t*
usbd_serial_internal_string_descriptor(void)
{
    static struct {
        uint8_t bLength;
        uint8_t bDescriptorType;
        uint16_t wData[0x18];
    } __ALIGNED(2) descr = {
        .bLength = 0,
        .bDescriptorType = USB_DESCR_TYPE_STRING,
    };

    if (descr.bLength > 0)
        return (const usb_string_descriptor_t*) &descr;

    uint8_t idx = 0;
    for (uint8_t i = 0; i < 12; i++) {
        uint8_t tmp = *((uint8_t*) (UID_BASE + i));
        descr.wData[idx++] = to_hex(tmp >> 4);
        descr.wData[idx++] = to_hex(tmp);
    }

    descr.bLength = sizeof(descr.bLength) + sizeof(descr.bDescriptorType) +
        idx * sizeof(uint16_t);

    return (const usb_string_descriptor_t*) &descr;
}
