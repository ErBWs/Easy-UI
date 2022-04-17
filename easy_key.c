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
__attribute__((weak)) void EasyKey_Sync(EasyKey_t *key)
{
    key->value = !GPIO_ReadInDataBit(key->GPIOX,key->init.Pins);

    // Don't modify this
    key->preval = key->value;
}


__attribute__((weak)) void EasyKey_PressCallback(EasyKey_t *key)
{
    
}


__attribute__((weak)) void EasyKey_HoldCallback(EasyKey_t *key)
{
    
}


__attribute__((weak)) void EasyKey_MultiClickCallback(EasyKey_t *key)
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

void EasyKey_Init(EasyKey_t *key, uint32_t pin, uint8_t num, uint8_t period)
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
    key->GPIOX = ( (GPIO_Type*)(AHB1_2_BASE + (pin&0xBF - 1) * 1024) );
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
void EasyKey_Handler()
{
    for (EasyKey_t *key = head; key != NULL; key = key->next)
    {
        // Get key value
        EasyKey_Sync(key);

        // Time counter
        if(!key->value)
        {
            if(key->state != dither)
            {
                key->hold_time = 0;
            }
        }
        if (key->value & key->preval)
        {
            key->hold_time += key->timer;
        }

        if (key->state == pre_click | key->state == in_click)
        {
            key->interval_time += key->timer;
        }
        else
        {
            key->interval_time = 0;
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
                        key->state = pre_click;
                        key->click_count++;
                    }
                    else
                    {
                        key->state = release;
                    }
                }
                break;
            }

            case pre_click:
            {
                if (key->interval_time < INTERVAL_THRESHOLD)
                {
                    if (key->hold_time > PRESS_THRESHOLD)
                    {
                        key->state = multi_click;
                        key->click_count++;
                    }
                    break;
                }

                key->state = press;
                break;
            }

            case in_click:
            {
                if (key->interval_time < INTERVAL_THRESHOLD)
                {
                    if (key->hold_time > PRESS_THRESHOLD)
                    {
                        key->state = multi_click;
                        key->click_count++;
                    }
                }
                else
                {
                    EasyKey_MultiClickCallback(key);
                    key->state = release;
                }
                break;
            }

            case press:
            {
                EasyKey_PressCallback(key);
                if (!key->value)
                {
                    key->state = release;
                }                
                break;
            }  
    
            case hold:
            {
                if (!key->value)
                {
                    EasyKey_HoldCallback(key);
                    key->state = release;
                }
                break;
            }

            case multi_click:
            {
                if (!key->value)
                {
                    key->state = in_click;
                }
                break;
            }
            
            default:
                break;
        }
    }
}
