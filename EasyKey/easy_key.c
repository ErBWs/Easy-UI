/*!
 * Copyright (c) 2022, ErBW_s
 * All rights reserved.
 *
 * @author  Baohan
 */

#include "easy_key.h"

/*!
 * @brief       Callback
 * @param       *key        Key linked list
 * @return      void
 * @note        Modify this part
 */
//--------------------------------------------------------------------------
__attribute__((weak)) void SyncValue(EasyKey_t *key)
{
    key->value = !gpio_get_level(key->pin);

    // Don't modify this
    key->preVal = key->value;
}


__attribute__((weak)) void PressCallback(EasyKey_t *key)
{
    key->isPressed = true;
}


__attribute__((weak)) void HoldCallback(EasyKey_t *key)
{
    key->isHold = true;
}


__attribute__((weak)) void MultiClickCallback(EasyKey_t *key)
{
    key->isMultiClick = true;
}


__attribute__((weak)) void ReleaseCallback(EasyKey_t *key)
{
    key->isMultiClick = false;
    key->isPressed = false;
    key->isHold = false;
}
//--------------------------------------------------------------------------


EasyKey_t *head = NULL, *tail = NULL;
/*!
 * @brief       Key linked list init
 * @param       key         Linked list's node
 * @param       pin         Gpio pin
 * @param       num         Gpio num
 * @param       period      Scanner period(ms), should be the same as the Timer period
 * @return      void
 * @sample      key_init(&key1, 'G', 1, 10)     Init G1 as key input, 10ms scanner period
 */
void EasyKeyInit(EasyKey_t *key, gpio_pin_enum _pin)
{
    key->state = release;
    key->next = NULL;
    key->holdTime = 0;
    key->intervalTime = 0;
    key->pin = _pin;
    
// Modify this part --------------------------------------------------------

    // Type your 3rd party driver here
    gpio_init(_pin, GPI, 0,GPI_PULL_UP);

//--------------------------------------------------------------------------

    if(head == NULL)
    {
        head = key;
        tail = key;
    } else
    {
        tail->next = key;
        tail = tail->next;
    }
}


uint8_t multiClickSwitch = 0;
/*!
 * @brief       Key interrupt handler
 * @param       void
 * @return      void
 * @note        Don't modify
 */
void EasyKeyHandler(uint8_t timer)
{
    for (EasyKey_t *key = head; key != NULL; key = key->next)
    {
        // Get key value
        SyncValue(key);

        // Time counter
        if(!key->value)
        {
            if(key->state != dither && key->state != hold)
                key->holdTime = 0;
        }
        if (key->value & key->preVal)
            key->holdTime += timer;

        if (key->state == preClick | key->state == inClick)
            key->intervalTime += timer;
        else
            key->intervalTime = 0;

        // Events
        switch (key->state)
        {
            case release:
            {
                ReleaseCallback(key);
                key->clickCnt = 0;

                if (key->value)
                    key->state = dither;
                break;
            }

            case dither:
            {
                if (key->holdTime > HOLD_THRESHOLD)
                    key->state = hold;

                if (!key->value)
                {
                    if (key->holdTime > PRESS_THRESHOLD && key->holdTime < HOLD_THRESHOLD)
                    {
                        key->state = preClick;
                        key->clickCnt++;
                    } else
                    {
                        key->state = release;
                    }
                }
                break;
            }

            case preClick:
            {
                if (multiClickSwitch)
                {
                    if (key->intervalTime < INTERVAL_THRESHOLD)
                    {
                        if (key->holdTime > PRESS_THRESHOLD)
                        {
                            key->state = multiClick;
                            key->clickCnt++;
                        }
                        break;
                    }
                }

                key->state = press;
                break;
            }

            case inClick:
            {
                if (key->intervalTime < INTERVAL_THRESHOLD)
                {
                    if (key->holdTime > PRESS_THRESHOLD)
                    {
                        key->state = multiClick;
                        key->clickCnt++;
                    }
                } else
                {
                    MultiClickCallback(key);
                    key->state = release;
                }
                break;
            }

            case press:
            {
                PressCallback(key);
                if (!key->value)
                    key->state = release;
                break;
            }  
    
            case hold:
            {
                if (!key->value)
                {
                    HoldCallback(key);
                    key->state = release;
                }
                break;
            }

            case multiClick:
            {
                if (!key->value)
                    key->state = inClick;
                break;
            }
            
            default:
                break;
        }
    }
}