/*!
 * Copyright (c) 2022, ErBW_s
 * All rights reserved.
 *
 * @author  Baohan
 */

#ifndef _easy_key_h_
#define _easy_key_h_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "zf_driver_gpio.h"
#include "zf_driver_delay.h"

#define FILTER_TIME_US          100     // Dithering elimination
#define UPDATE_KEY_STATE_MS     1       // Update key state once per (x) ms
#define HOLD_THRESHOLD_MS       300     // Time longer than this is considered as "hold"
#define INTERVAL_THRESHOLD_MS   140     // Trigger time interval less than this is considered as "multiClick"

typedef struct EasyKey_typedef
{
    // Internal call
    uint8_t value, cacheValue;      // Press:0  Not press:1
    uint8_t preValue;
    struct EasyKey_typedef *next;
    gpio_pin_enum pin;
    uint32_t holdTime, intervalTime;

    enum
    {
        down,
        up,
        pressed,
        released
    } state;

    // User call
    bool isPressed;
    bool isHold;
    bool isMultiClick;
    uint8_t clickState;
} EasyKey_t;

void EasyKeyInit(EasyKey_t *key, gpio_pin_enum _pin);
void EasyKeyScanKeyState();

extern bool multiClickSwitch;
void EasyKeyUserApp();

#ifdef __cplusplus
}
#endif

#endif
