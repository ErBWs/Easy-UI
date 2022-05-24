/*
 * @file    easy_key.h
 * @author  ErBW_s
 * @date    2022/04/09
 */

#ifndef _easy_key_h_
#define _easy_key_h_

#include <iostream>
#include <forward_list>

extern "C" {
#include "hal_gpio.h"
#include "hal_rcc.h"
#include "mm32f3277g.h"
}

using namespace std;

#define USE_HAL_DRIVER 1            // If you use 3rd party driver, change this def to 0

class EasyKey
{
private:
    uint8_t timer;              //Suggest using 10ms interrupt
    uint8_t value, preVal;      // Press:1  Not press:0
    uint32_t intervalTime;

    GPIO_Init_Type init;
    GPIO_Type *GPIOX;

    static const uint32_t PRESS_THRESHOLD = 10;        // Dithering elimination
    static const uint32_t HOLD_THRESHOLD = 200;        // Time longer than this is considered as "hold"
    static const uint32_t INTERVAL_THRESHOLD = 150;    // Trigger time interval less than this is considered as "multiclick"

public:
    EasyKey(uint32_t _pin, uint8_t _num, uint8_t _period);
    ~EasyKey();

    static void SyncValue(EasyKey &_key);
    static void PressCallback(EasyKey &_key);
    static void HoldCallback(EasyKey &_key);
    static void MultiClickCallback(EasyKey &_key);

    static void Handler(void);
    
    uint32_t holdTime;
    uint32_t clickCnt;
    
    enum
    {
        // Internal call
        release,
        dither,
        preClick,
        inClick,

        // User call
        press,
        hold,
        multiClick
    } state;
};

#endif
