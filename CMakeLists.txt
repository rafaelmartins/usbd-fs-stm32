# SPDX-FileCopyrightText: 2024 Rafael G. Martins <rafael@rafaelmartins.eng.br>
# SPDX-License-Identifier: BSD-3-Clause

if(NOT TARGET usbd-fs-stm32)
    add_library(usbd-fs-stm32 INTERFACE)

    target_sources(usbd-fs-stm32 INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/src/usbd.c
        ${CMAKE_CURRENT_LIST_DIR}/include/usbd.h
        ${CMAKE_CURRENT_LIST_DIR}/include/usb-std-audio.h
        ${CMAKE_CURRENT_LIST_DIR}/include/usb-std-hid.h
        ${CMAKE_CURRENT_LIST_DIR}/include/usb-std-midi.h
        ${CMAKE_CURRENT_LIST_DIR}/include/usb-std.h
    )

    target_include_directories(usbd-fs-stm32 INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/include
    )
endif()
