# Easy-Key

基于灵动Hal库编写的一个按键库，支持短按、长按和连点



## 使用方法

- 将`easy_key.c`与`easy_key.h`加入到你的工程文件中
- 按需修改`easy_key.h`文件中的宏

>`USE_HAL_DRIVER`：是否使用hal库，如果使用第三方库将此值改为0
>
>`PRESS_THRESHOLD`：消抖检测时间，低于此时间的点击会被认为是抖动
>
>`HOLD_THRESHOLD`：*长按检测时间，长于此时间的点击会被认为是长按*
>
>`INTERVAL_THRESHOLD`：点击间隔时间，两次点击间隔低于此时间的点击会被认为是连点

* 根据自己芯片的库文件修改`EasyKey_Sync`文件中的**读取GPIO口电平**函数
* 根据自己芯片的库文件修改`EasyKey_Init`文件中的**GPIO初始化**方式
* 根据需要改写回调函数(例如增加使用串口发送按键状态信息功能)

>`EasyKey_PressCallback`：短按回调函数
>
>`EasyKey_HoldCallback`：长按回调函数
>
>`EasyKey_MultiClickCallback`：连点回调函数

* 为每一个按键定义一个`EasyKey_t`句柄

* 为**每一个按键**调用`EasyKey_Init`函数

* 使用**定时器中断**调用`EasyKey_Handler`函数，我个人推荐中断使用**10ms**触发一次



## 说明

### 接口函数移植

按键触发时GPIO口电平应为**高电平**

如果发现不能使用可以尝试定义句柄时**不使用**`EasyKey_t *`；

而在`EasyKey_Init`初始化时使用`&key`

### 使用第三方库

修改`USE_HAL_DRIVER`的值为0；

在`Type your 3rd party driver here`注释处键入第三方库GPIO初始化函数；

修改`EasyKey_Init`的函数参数(如果需要)

### 获取按键状态

建议使用**触发时间相同**的**定时器中断**获取按键状态

使用`key.state` / `key->state`获取值即可，其中供用户使用的值为**最后三个**：

> `press`：短按状态
>
> `hold`：长按状态
>
> `multi_click`：连点状态

在长按回调函数中调用`key.hold_time` / `key->hold_time`可获取**长按持续时间**以供使用

在连点回调函数中调用`key.click_count` / `key->click_count`可获取**连点次数**以供使用

### 点按延时

触发短按状态会有延时，因为要检测是否为连点，这样的好处是触发连点前可以不用触发短按

延时受到`INTERVAL_THRESHOLD`的影响

不希望有延时的可以修改`EasyKey_Handler`或使用Zhewana 编写的 [CommonKey](https://github.com/Zhewana/CommonKey)

## 其他

编写代码时参考了 Zhewana 编写的 [CommonKey](https://github.com/Zhewana/CommonKey) 以及 [逐飞](https://gitee.com/seekfree) 的开源按键库

等我有stm32板子之后可能会上传一份stm32版的按键库

代码仅经过初步验证，尚未经过实例验证，稳定性可能不是很好，欢迎大家提交修改或建议
