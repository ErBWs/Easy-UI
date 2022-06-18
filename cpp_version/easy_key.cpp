/*
 * @file    easy_key.c
 * @author  ErBW_s
 * @date    2022/04/09
 */

#include "easy_key.h"

forward_list<EasyKey> easyKeyList;
/*
 * @brief       EasyKey constructor
 * @param       _pin        Gpio pin
 * @param       _num        Gpio num
 * @param       _period     Scanner period(ms), should be the same as the Timer period
 * @return      void
 * @sample      key_init(&key1, 'G', 1, 10)     Init G1 as key input, 10ms scanner period
 */
EasyKey::EasyKey(uint32_t _pin, uint8_t _num, uint8_t _period)
{
    state = release;
    timer = _period;
    holdTime = 0;
    intervalTime = 0;
    
// Modify this part --------------------------------------------------------
#if USE_HAL_DRIVER

    // Enable RCC GPIO clock
    RCC->AHB1ENR |= (1u << (_pin & 0xBF - 1));

    // GPIO init
	init.PinMode = GPIO_PinMode_In_PullUp;
	init.Pins = (1u << _num);
	init.Speed = GPIO_Speed_50MHz;
    GPIOX = ((GPIO_Type *)(AHB1_2_BASE + (_pin & 0xBF - 1) * 1024));
    GPIO_Init(GPIOX, &init);

#else

    // Type your 3rd party driver here

#endif
//--------------------------------------------------------------------------

    static auto it = easyKeyList.before_begin();
    it = easyKeyList.emplace_after(it, *this);
}


/*
 * @brief       EasyKey destructor
 * @param       void
 * @return      void
 * @note        Don't modify
 */
EasyKey::~EasyKey()
{
    easyKeyList.clear();
}


/*
 * @brief       Callback
 * @param       _key        Key linked list
 * @return      void
 * @note        Modify this part
 */
//--------------------------------------------------------------------------
inline void EasyKey::SyncValue(EasyKey &_key)
{
    _key.value = !GPIO_ReadInDataBit(_key.GPIOX, _key.init.Pins);

    // Don't modify this
    _key.preVal = _key.value;
}


inline void EasyKey::PressCallback(EasyKey &_key)
{
    
}


inline void EasyKey::HoldCallback(EasyKey &_key)
{
    
}


inline void EasyKey::MultiClickCallback(EasyKey &_key)
{

}
//--------------------------------------------------------------------------


/*
 * @brief       Key interrupt handler
 * @param       void
 * @return      void
 * @note        Don't modify
 */
void EasyKey::Handler(void)
{
    for (auto &key : easyKeyList)
    {
        // Get key value
        SyncValue(key);

        // Time counter
        if(!key.value)
        {
            if(key.state != dither && key.state != hold)
                key.holdTime = 0;
        }
        if (key.value & key.preVal)
            key.holdTime += key.timer;

        if (key.state == preClick | key.state == inClick)
            key.intervalTime += key.timer;
        else
            key.intervalTime = 0;

        // Events
        switch (key.state)
        {
            case release:
            {
                key.clickCnt = 0;

                if (key.value)
                    key.state = dither;
                break;
            }

            case dither:
            {
                if (key.holdTime > HOLD_THRESHOLD)
                    key.state = hold;

                if (!key.value)
                {
                    if (key.holdTime > PRESS_THRESHOLD && key.holdTime < HOLD_THRESHOLD)
                    {
                        key.state = preClick;
                        key.clickCnt++;
                    } else
                    {
                        key.state = release;
                    }
                }
                break;
            }

            case preClick:
            {
                if (key.intervalTime < INTERVAL_THRESHOLD)
                {
                    if (key.holdTime > PRESS_THRESHOLD)
                    {
                        key.state = multiClick;
                        key.clickCnt++;
                    }
                    break;
                }

                key.state = press;
                break;
            }

            case inClick:
            {
                if (key.intervalTime < INTERVAL_THRESHOLD)
                {
                    if (key.holdTime > PRESS_THRESHOLD)
                    {
                        key.state = multiClick;
                        key.clickCnt++;
                    }
                } else
                {
                    MultiClickCallback(key);
                    key.state = release;
                }
                break;
            }

            case press:
            {
                PressCallback(key);
                if (!key.value)
                    key.state = release;
                break;
            }  
    
            case hold:
            {
                if (!key.value)
                {
                    HoldCallback(key);
                    key.state = release;
                }
                break;
            }

            case multiClick:
            {
                if (!key.value)
                    key.state = inClick;
                break;
            }
            
            default:
                break;
        }
    }
}
