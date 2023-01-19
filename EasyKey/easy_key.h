/*!
 * Copyright (c) 2022, ErBW_s
 * All rights reserved.
 *
 * @author  Baohan
 */

#ifndef _easy_key_h_
#define _easy_key_h_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "zf_driver_gpio.h"

#define PRESS_THRESHOLD 20      // Dithering elimination
#define HOLD_THRESHOLD 100      // Time longer than this is considered as "hold"
#define INTERVAL_THRESHOLD 80  // Trigger time interval less than this is considered as "multiclick"

typedef struct EasyKey_typedef
{
    // Internal call
    uint8_t value, preVal;      // Press:1  Not press:0
    uint32_t intervalTime;
    struct EasyKey_typedef *next;
    gpio_pin_enum pin;
    uint32_t holdTime;
    uint32_t clickCnt;
    enum
    {
        release,
        dither,
        preClick,
        inClick,
        press,
        hold,
        multiClick
    } state;

    // User call
    bool isPressed;
    bool isHold;
    bool isMultiClick;
} EasyKey_t;

extern uint8_t multiClickSwitch;

void EasyKeyInit(EasyKey_t *key, gpio_pin_enum _pin);
void EasyKeyHandler(uint8_t timer);

__attribute__((weak)) void SyncValue(EasyKey_t *key);
__attribute__((weak)) void PressCallback(EasyKey_t *key);
__attribute__((weak)) void HoldCallback(EasyKey_t *key);
__attribute__((weak)) void MultiClickCallback(EasyKey_t *key);
__attribute__((weak)) void ReleaseCallback(EasyKey_t *key);

#endif
