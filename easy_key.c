/*
 * @file    easy_key.c
 * @author  ErBW_s
 * @date    2022/04/09
 */

#include "easy_key.h"

/*
 * @brief       Callback
 * @param       *key        Key linked list
 * @return      void
 * @note        Modify this part
 */
//--------------------------------------------------------------------------
__attribute__((weak)) void key_sync(EasyKey_t *key)
{
    key->value = !GPIO_ReadInDataBit(key->GPIOX,key->init.Pins);

    // Don't modify this
    key->preval = key->value;
}


__attribute__((weak)) void key_PressCallback(EasyKey_t *key)
{
    
}


__attribute__((weak)) void key_HoldCallback(EasyKey_t *key)
{
    
}


__attribute__((weak)) void key_MultiClickCallback(EasyKey_t *key)
{
    
}
//--------------------------------------------------------------------------


/*
 * @brief       Key linked list init
 * @param       key         Linked list's node
 * @param       pin         Gpio pin
 * @param       num         Gpio num
 * @param       period      Scanner period(ms), should be the same as the Timer period
 * @return      void
 * @sample      key_init(&key1, 'G', 1, 10)     Init G1 as key input, 10ms scanner period
 */
EasyKey_t *head = NULL, *tail = NULL;

void key_init(EasyKey_t *key, uint32_t pin, uint8_t num, uint8_t period)
{
    key->state = release;
    key->next = NULL;
    key->timer = period;
    key->hold_time = 0;
    key->interval_time = 0;
    
// Modify this part --------------------------------------------------------
  #if USE_HAL_DRIVER

    // Enable RCC GPIO clock
    RCC->AHB1ENR |= (1u << (pin&0xBF - 1));

    // GPIO init
	key->init.PinMode = GPIO_PinMode_In_PullUp;
	key->init.Pins = (1u << num);
	key->init.Speed = GPIO_Speed_50MHz;
    key->GPIOX = ((GPIO_Type*)(AHB1_2_BASE + (pin&0xBF - 1) * 1024));
	GPIO_Init(key->GPIOX, &key->init);

  #elif

    // Type your 3rd party driver here

  #endif
//--------------------------------------------------------------------------

    if(head == NULL)
    {
        head = key;
        tail = key;
    }
    else
    {
        tail->next = key;
        tail = tail->next;
    }
}


/*
 * @brief       Key interrupt handler
 * @param       void
 * @return      void
 * @note        Don't modify
 */
void key_handler()
{
    for (EasyKey_t *key = head; key != NULL; key = key->next)
    {
        // Get key value
        key_sync(key);

        // Time counter
        if (key->value & key->preval)
        {
            key->hold_time += key->timer;
        }

        if (key->value == 0 && key->preval == 0)
        {
            key->interval_time += key->timer;
        }

        // Events
        switch (key->state)
        {
            case release:
            {
                key->click_count = 0;

                if (key->value)
                {
                    key->state = dither;
                }
                break;
            }

            case dither:
            {
                if (key->hold_time > HOLD_THRESHOLD)
                {
                    key->state = hold;
                }
                if (!key->value)
                {
                    if (key->hold_time > PRESS_THRESHOLD && key->hold_time < HOLD_THRESHOLD)
                    {
                        key->state = preclick;
                        key->interval_time = 0;
                        key->hold_time = 0;
                    }
                    else
                    {
                        key->state = release;
                        key->hold_time = 0;
                    }
                }
                break;
            }

            case preclick:
            {
                static uint8_t t = 0;
                if (key->interval_time < INTERVAL_THRESHOLD)
                {
                    if (key->value)
                    {
                        key->state = multi_click;
                        key->interval_time = 0;
                        t = 0;
                    }
                    break;
                }

                key->state = press;
                break;
            }

            case press:
            {
                static uint8_t i = 0;
                key_PressCallback(key);
                if(!key->value)
                {
                    // Increase status show time
                    if (i < 5)
                    {
                        i++;
                        break;
                    }
                    i = 0;
                    
                    key->state = release;
                    key->hold_time = 0;
                }                
                break;
            }  

            case hold:
            {
                key_HoldCallback(key);
                if(!key->value)
                {
                    key->state = release;
                    key->hold_time = 0;
                }
                break;
            }

            case multi_click:
            {
                static uint8_t j = 0;
                key_MultiClickCallback(key);
                key->click_count++;

                // Increase status show time
                if (j < 3)
                {
                    j++;
                    break;
                }
                j = 0;

                if(key->interval_time > INTERVAL_THRESHOLD)
                {
                    key->state = release;
                    key->hold_time = 0;
                    key->interval_time = 0;
                    key->click_count = 0;
                }
                break;
            }

            default:
                break;
        }
    }
}