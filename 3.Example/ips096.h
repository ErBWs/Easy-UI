/*!
 * Copyright (c) 2023, ErBW_s
 * All rights reserved.
 * 
 * @author  Baohan
 */

#ifndef ips096_h_
#define ips096_h_

#ifdef __cplusplus
extern "C"
{
#endif

#include "zf_common_clock.h"
#include "zf_common_debug.h"
#include "zf_common_font.h"
#include "zf_common_function.h"
#include "zf_driver_delay.h"
#include "zf_driver_soft_spi.h"
#include "zf_driver_spi.h"

#include "zf_driver_timer.h"
#include <math.h>
extern uint16_t time;

#define IPS096_SPI_SPEED                (72 * 1000 * 1000)                      // 硬件 SPI 速率 这里设置为系统时钟二分频
#define IPS096_SPI                      (SPI_2)                                 // 硬件 SPI 号
#define IPS096_SCL_PIN                  (SPI2_MAP0_SCK_B13)                           // 硬件 SPI SCK 引脚
#define IPS096_SDA_PIN                  (SPI2_MAP0_MOSI_B15)                          // 硬件 SPI MOSI 引脚

#define IPS096_RST_PIN                  (B7 )                                   // 液晶复位引脚定义
#define IPS096_DC_PIN                   (D7 )                                   // 液晶命令位引脚定义
#define IPS096_CS_PIN                   (D4 )                                   // CS 片选引脚
#define IPS096_BLK_PIN                  (D0 )                                   // 液晶背光引脚定义

#define IPS096_DEFAULT_DISPLAY_DIR      (IPS096_CROSSWISE_180)                  // 默认的显示方向
#define IPS096_DEFAULT_PENCOLOR         (RGB565_WHITE)                            // 默认的画笔颜色
#define IPS096_DEFAULT_BGCOLOR          (RGB565_BLACK)                          // 默认的背景颜色
#define IPS096_DEFAULT_DISPLAY_FONT     (IPS096_6X8_FONT)                      // 默认的字体模式

#define IPS096_DC(x)                    ((x) ? (gpio_high(IPS096_DC_PIN)) : (gpio_low(IPS096_DC_PIN)))
#define IPS096_RST(x)                   ((x) ? (gpio_high(IPS096_RST_PIN)) : (gpio_low(IPS096_RST_PIN)))
#define IPS096_CS(x)                    ((x) ? (gpio_high(IPS096_CS_PIN)) : (gpio_low(IPS096_CS_PIN)))
#define IPS096_BLK(x)                   ((x) ? (gpio_high(IPS096_BLK_PIN)) : (gpio_low(IPS096_BLK_PIN)))

#define CIRCLE_UPPER_RIGHT      0x01
#define CIRCLE_UPPER_LEFT       0x02
#define CIRCLE_LOWER_LEFT       0x04
#define CIRCLE_LOWER_RIGHT      0x08
#define CIRCLE_DRAW_ALL         (CIRCLE_UPPER_RIGHT | CIRCLE_UPPER_LEFT | CIRCLE_LOWER_LEFT | CIRCLE_LOWER_RIGHT)

extern uint16 IPS096_penColor;
extern uint16 IPS096_backgroundColor;

typedef enum
{
    NORMAL = 1,
    XOR
} IPS096_ColorMode_e;

typedef enum
{
    IPS096_PORTAIT                      = 0,                                    // 竖屏模式
    IPS096_PORTAIT_180                  = 1,                                    // 竖屏模式  旋转180
    IPS096_CROSSWISE                    = 2,                                    // 横屏模式
    IPS096_CROSSWISE_180                = 3,                                    // 横屏模式  旋转180
}ips096_dir_enum;

typedef enum
{
    IPS096_6X8_FONT                     = 0,                                    // 6x8      字体
    IPS096_8X16_FONT                    = 1,                                    // 8x16     字体
    IPS096_16X16_FONT                   = 2,                                    // 16x16    字体 目前不支持
}ips096_font_size_enum;

void IPS096_SendBuffer();
void IPS096_ClearBuffer();

void IPS096_SetDrawColor(IPS096_ColorMode_e mode);
void IPS096_DrawPoint (int16 x, int16 y, uint16 color);
void IPS096_DrawLine (int16 x_start, int16 y_start, int16 x_end, int16 y_end, uint16 color);
void IPS096_ShowChar(int16 x, int16 y, char dat);
void IPS096_ShowStr (int16 x, int16 y, const char dat[]);
void IPS096_ShowInt(int16 x, int16 y, int32 dat, uint8 num);
void IPS096_ShowUint(int16 x, int16 y, uint32 dat, uint8 num);
void IPS096_ShowFloat(int16 x, int16 y, float dat, uint8 num, uint8 pointnum);

extern bool reversedColor;
void IPS096_ModifyColor();

void IPS096_DrawCircle(int16_t x, int16_t y, uint16_t r, uint16_t color, uint8_t section);
void IPS096_DrawDisc(int16_t x, int16_t y, uint16_t r, uint16_t color, uint8_t section);
void IPS096_DrawFrame(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t color);
void IPS096_DrawBox(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t color);
void IPS096_DrawRFrame(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t color, uint8_t r);
void IPS096_DrawRBox(int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t color, uint8_t r);
void IPS096_DrawRBoxWithBlur(int16_t x, int16_t y, uint16_t width, uint16_t height);

void IPS096_ShowBMP(int16_t x, int16_t y, uint16_t width, uint16_t height, const uint8_t *pic);
void IPS096_ShowGrayImage(uint16_t x, uint16_t y, const uint8_t *image, uint16_t width, uint16_t height, uint16_t dis_width,
                          uint16_t dis_height, uint8_t threshold);

void IPS096_Init();

#ifdef __cplusplus
}
#endif

#endif
