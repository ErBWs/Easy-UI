# Easy-Key

A key driver based on hal diver of MindMotion, support short press, hold and multiclick

## [中文](./README_CN.md)

>Unless stated, all refer to the C version of key dirver

## User manual

- Adding files to your projects according to your language

- Modify definitions or constants(C++) in`easy_key.h`

>`USE_HAL_DRIVER`：Use hal or not, if you use 3rd party driver, change this to 0
>
>`PRESS_THRESHOLD`：Dithering elimination, press time less than this is considered dithering
>
>`HOLD_THRESHOLD`：Hold threshold, press time longer than this is considered as "hold"
>
>`INTERVAL_THRESHOLD`：Time between two click, time less than this is considered as "multiclick"

* Change the **read GPIO pin level** function in `SyncValue` according to your mcu

* Change the **GPIO initialization** function in `EasyKeyInit` (C++: `EasyKey::EasyKey`) according to your mcu

* Modify the callback function (e.g. Send key state using UART)

>`PressCallback`：Short press callback function
>
>`HoldCallback`：Hold callback function
>
>`MultiClickCallback`：Multiclick callback function

* Use `EasyKey_t` to define **all the keys**

* Call `EasyKeyInit` function for **all the keys**

>(C++)For C++ version，you only need to call `EasyKey::EasyKey` function for **all the keys**

* Call `EasyKey_Handler` (C++: `EasyKey::Handler`) function in **timer interrupt**, recommend **10ms**

## Note

### Interface function migration

When key is pressed, the pin level should be **high**

When you find it not work, try **not** to use `EasyKey_t *`,

use `&key` when initializing with `EasyKeyInit`

### Use 3rd party library

Change `USE_HAL_DRIVER` to 0

Type your 3rd party driver at `Type your 3rd party driver here` annotation

Modify the parameter of `EasyKeyInit` (C++: `EasyKey::EasyKey`) (if needed)

### Get key state

Recommend using **timer interrupt** with **same trigger time** to get key state

Use `key.state` (C/C++) / `key->state` (C/C++) to get key state，the following three are for the user：

> `press`：short press state
>
> `hold`：hold state
>
> `multiClick`：multiclick state

Call `key.holdTime` (C/C++) / `key->holdTime` (C/C++) in **hold callback** can have access to **total hold time**

Call `key.clickCnt` (C/C++) / `key->clickCnt` (C/C++) in **multiclick callback** can have access to **total click counts**

* **Total** hold time and click counts **have** to be obtained by using **callback function**!

### Short press delay

There are delay when detecting short press delay due to the need of multiclick detection,

the delay time is influenced by `INTERVAL_THRESHOLD`

If you want no delay, change the `EasyKeyHandler` (C++: `EasyKey::Handler`) function

## Other

Thank for Zhewana's [CommonKey](https://github.com/Zhewana/CommonKey) and Seekfree's [Opensource key driver](https://gitee.com/seekfree)

I will keep undating this driver

There will probably be a stm32 version of this driver when I get one stm32 board

Comments or suggestions are welcomed