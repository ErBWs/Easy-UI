/*!
 * Copyright (c) 2022, ErBW_s
 * All rights reserved.
 *
 * @author  Baohan
 */

#include "easy_key.h"

bool multiClickSwitch = false;
EasyKey_t *head = NULL, *tail = NULL;

/*!
 * @brief   Callbacks
 *
 * @param   key         Key linked list
 * @return  void
 *
 * @note    Modify this part
 */
void PressCallback(EasyKey_t *key)
{
    key->isMultiClick = false;
    key->isPressed = true;
    key->isHold = false;
}
void HoldCallback(EasyKey_t *key)
{
    key->isMultiClick = false;
    key->isPressed = false;
    key->isHold = true;
}
void MultiClickCallback(EasyKey_t *key)
{
    key->isMultiClick = true;
    key->isPressed = false;
    key->isHold = false;
}
void ReleaseCallback(EasyKey_t *key)
{
    key->isMultiClick = false;
    key->isPressed = false;
    key->isHold = false;
}


/*!
 * @brief   EasyKey user app
 *
 * @param   void
 * @return  void
 *
 * @note    Modify this part
 */
void EasyKeyUserApp()
{
    for (EasyKey_t *key = head; key != NULL; key = key->next)
    {
        ReleaseCallback(key);

        // Press and hold callback
        if (key->state == up && key->holdTime >= HOLD_THRESHOLD_MS)
            HoldCallback(key);
        if (key->state == up && key->holdTime < HOLD_THRESHOLD_MS)
        {
            if (multiClickSwitch == false)
            {
                PressCallback(key);
                continue;
            }
            if (key->clickState == 0)
                key->clickState = 1;
        }

        // Skip multiple click judgement
        if (multiClickSwitch == false)
        {
            continue;
        }

        /*
         * clickState = 0: Normal state
         * clickState = 1: Waiting for multiple click
         * clickState = 2: Interval time meet multiple click requirement, and need to judge hold time
         */
        if (key->clickState == 1 && key->state == down && key->intervalTime < INTERVAL_THRESHOLD_MS)
        {
            // Enter click state 2
            key->clickState = 2;
        }
        if (key->clickState == 1 && key->intervalTime > INTERVAL_THRESHOLD_MS)
        {
            // Interval time too long, trigger PressCallBack
            PressCallback(key);
            key->clickState = 0;
        }
        if (key->clickState == 2 && key->state == up && key->holdTime < HOLD_THRESHOLD_MS)
        {
            // Meet all requirements, trigger MultiClickCallBack
            MultiClickCallback(key);
            key->clickState = 0;
        }
    }
}


/*!
 * @brief       Key linked list init
 *
 * @param       key         Linked list's node
 * @param       _pin        Gpio pin
 * @return      void
 */
void EasyKeyInit(EasyKey_t *key, gpio_pin_enum _pin)
{
    key->state = released;
    key->next = NULL;
    key->holdTime = 0;
    key->intervalTime = 0;
    key->pin = _pin;
    key->preValue = 1;

    // GPIO init
    gpio_init(_pin, GPI, 0,GPI_PULL_UP);

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


/*!
 * @brief   Dither elimination
 *
 * @param   timeUs      Delay time(us)
 * @return  void
 */
void DebounceFilter(uint8_t timeUs)
{
    for (EasyKey_t *key = head; key != NULL; key = key->next)
    {
        key->cacheValue = gpio_get_level(key->pin);
    }

    system_delay_us(timeUs);

    uint8_t mask;
    for (EasyKey_t *key = head; key != NULL; key = key->next)
    {
        key->value = gpio_get_level(key->pin);
        mask = key->value ^ key->cacheValue;
        key->value |= mask;
    }
}


/*!
 * @brief       Update key state
 *
 * @param       void
 * @return      void
 */
void EasyKeyScanKeyState()
{
    DebounceFilter(FILTER_TIME_US);

    for (EasyKey_t *key = head; key != NULL; key = key->next)
    {
        // Update key state
        if (key->preValue > key->value)
            key->state = down;
        else if (key->preValue < key->value)
            key->state = up;
        else if (key->value == 0)
            key->state = pressed;
        else
            key->state = released;

        // Time counter
        switch (key->state)
        {
        case down:
            key->holdTime = 0;
            break;
        case up:
            key->intervalTime = 0;
            break;
        case pressed:
            key->holdTime += UPDATE_KEY_STATE_MS;
            break;
        default:
            if (key->intervalTime >= 3000)
                key->intervalTime = 3000;
            else
                key->intervalTime += UPDATE_KEY_STATE_MS;
            break;
        }

        // Store key value
        key->preValue = key->value;
    }
}