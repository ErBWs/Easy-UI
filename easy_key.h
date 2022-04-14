/*
 * @file    easy_key.h
 * @author  ErBW_s
 * @date    2022/04/09
 */

#ifndef _easy_key_h_
#define _easy_key_h_

#include <stdint.h>
#include "hal_gpio.h"
#include "hal_rcc.h"
#include "mm32f3277g.h"

#define USE_HAL_DRIVER 1        // If you use 3rd party driver, change this def to 0
#define PRESS_THRESHOLD 10      // Dithering elimination
#define HOLD_THRESHOLD 200      // Time longer than this will be considered as "hold"
#define INTERVAL_THRESHOLD 150  // Trigger time interval less than this will be considered as "multi_click"

typedef struct EasyKey_typedef
{
    uint8_t timer;              //Suggest using 10ms interrupt
    uint8_t value, preval;      // Press:1  Not press:0
    uint32_t hold_time;
    uint32_t interval_time;
    uint32_t click_count;

    GPIO_Init_Type init;
    GPIO_Type *GPIOX;
    struct EasyKey_typedef *next;

    enum
    {
        // Internal call
        release,
        dither,
        pre_click,
        in_click,

        // User call
        press,
        hold,
        multi_click
    } state;

}EasyKey_t;

void EasyKey_Init(EasyKey_t *key, uint32_t pin, uint8_t num, uint8_t period);

void EasyKey_Handler();

__attribute__((weak)) void EasyKey_Sync(EasyKey_t *key);

__attribute__((weak)) void EasyKey_PressCallback(EasyKey_t *key);

__attribute__((weak)) void EasyKey_HoldCallback(EasyKey_t *key);

__attribute__((weak)) void EasyKey_MultiClickCallback(EasyKey_t *key);

#endif
