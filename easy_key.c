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
    key->value = !GPIO_ReadInDataBit(key->GPIOX, key->init.Pins);

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


EasyKey_t *head = NULL, *tail = NULL;
/*
 * @brief       Key linked list init
 * @param       key         Linked list's node
 * @param       pin         Gpio pin
 * @param       num         Gpio num
 * @param       period      Scanner period(ms), should be the same as the Timer period
 * @return      void
 * @sample      key_init(&key1, 'G', 1, 10)     Init G1 as key input, 10ms scanner period
 */
void EasyKey_Init(EasyKey_t *key, uint32_t pin, uint8_t num, uint8_t period)
{
    key->state = release;
    key->next = NULL;
    key->timer = period;
    key->holdTime = 0;
    key->intervalTime = 0;
    
// Modify this part --------------------------------------------------------
#if USE_HAL_DRIVER

    // Enable RCC GPIO clock
    RCC->AHB1ENR |= (1u << (pin & 0xBF - 1));

    // GPIO init
	key->init.PinMode = GPIO_PinMode_In_PullUp;
	key->init.Pins = (1u << num);
	key->init.Speed = GPIO_Speed_50MHz;
    key->GPIOX = ( (GPIO_Type*)(AHB1_2_BASE + (pin & 0xBF - 1) * 1024) );
	GPIO_Init(key->GPIOX, &key->init);

#elif

    // Type your 3rd party driver here

#endif
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
            if(key->state != dither && key->state != hold)
            {
                key->holdTime = 0;
            }
        }
        if (key->value & key->preval)
        {
            key->holdTime += key->timer;
        }

        if (key->state == preClick | key->state == inClick)
        {
            key->intervalTime += key->timer;
        } else
        {
            key->intervalTime = 0;
        }

        // Events
        switch (key->state)
        {
            case release:
            {
                key->clickCnt = 0;

                if (key->value)
                {
                    key->state = dither;
                }
                break;
            }

            case dither:
            {
                if (key->holdTime > HOLD_THRESHOLD)
                {
                    key->state = hold;
                }
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
                if (key->intervalTime < INTERVAL_THRESHOLD)
                {
                    if (key->holdTime > PRESS_THRESHOLD)
                    {
                        key->state = multiClick;
                        key->clickCnt++;
                    }
                    break;
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

            case multiClick:
            {
                if (!key->value)
                {
                    key->state = inClick;
                }
                break;
            }
            
            default:
                break;
        }
    }
}