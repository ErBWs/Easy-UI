/*!
 * Copyright (c) 2023, ErBW_s
 * All rights reserved.
 *
 * @author  Baohan
 */

#ifndef _EASYUI_SE_H_
#define _EASYUI_SE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "easy_key.h"
#include "zf_driver_uart.h"
#include "zf_driver_flash.h"
#include "zf_driver_adc.h"
#include "user_flash.h"
#include "zf_device_ips114.h"
#include "zf_device_tft180.h"
#include "zf_device_oled.h"
#include "zf_device_ips200.h"
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

// Modify this to fit your EasyKeyInit
#define KEY_UP          E1
#define KEY_DOWN        A15
#define KEY_FORWARD     E3
#define KEY_BACKWARD    D3
#define KEY_CONFIRM     D1

extern EasyKey_t keyUp, keyDown;           // Used to control value up and down
extern EasyKey_t keyForward, keyBackward;  // Used to control indicator movement
extern EasyKey_t keyConfirm;               // Used to change page or call function

// Operation response
extern uint8_t opnForward, opnBackward;
extern uint8_t opnEnter, opnExit, opnUp, opnDown;

#define KEY_NUM         5
#define ROTARY          0

#define BATTERY_ADC_PIN         ADC1_IN9_B1
#define LOWEST_BATTERY_VOLTAGE  11.1f

#define SCREEN_WIDTH            160
#define SCREEN_HEIGHT           128
#define FONT_WIDTH              6
#define FONT_HEIGHT             8
#define ITEM_HEIGHT             12
#define CHECK_BOX_OFFSET        2
#define SCROLL_BAR_WIDTH        4
#define ITEM_LINES              ((uint8_t)(SCREEN_HEIGHT / ITEM_HEIGHT))
#define MAX_LAYER               10

/*
 * 1:       tft180
 * 2:       ips114
 * 3:       ips200
 * other:   oled
 */
#define USE_SCREEN              1

#if (USE_SCREEN == 1)
#define EasyUIScreenInit()                                      (tft180_init())
#define EasyUIClear()                                           (tft180_clear())
#define EasyUIDisplayStr(x, y, str)                             (tft180_show_string(x, y, str))
#define EasyUIDisplayFloat(x, y, dat, num, pointNum)            (tft180_show_float(x, y, dat, num, pointNum))
#elif (USE_SCREEN == 2)
#define EasyUIScreenInit()                                      (ips114_init())
#define EasyUIClear()                                           (ips114_clear())
#define EasyUIDisplayStr(x, y, str)                             (ips114_show_string(x, y, str))
#define EasyUIDisplayFloat(x, y, dat, num, pointNum)            (ips114_show_float(x, y, dat, num, pointNum))
#elif (USE_SCREEN == 3)
#define EasyUIScreenInit()                                      (ips200_init())
#define EasyUIClear()                                           (ips200_clear())
#define EasyUIDisplayStr(x, y, str)                             (ips200_show_string(x, y, str))
#define EasyUIDisplayFloat(x, y, dat, num, pointNum)            (ips200_show_float(x, y, dat, num, pointNum))
#else
#define EasyUIScreenInit()                                      (oled_init())
#define EasyUIClear()                                           (oled_clear())
#define EasyUIDisplayStr(x, y, str)                             (oled_show_string(x, y, str))
#define EasyUIDisplayFloat(x, y, dat, num, pointNum)            (oled_show_float(x, y, dat, num, pointNum))
#endif

typedef     double      paramType;

typedef enum
{
    ITEM_PAGE_DESCRIPTION,
    ITEM_JUMP_PAGE,
    ITEM_SWITCH,
    ITEM_CHANGE_VALUE,
    ITEM_PROGRESS_BAR,
    ITEM_RADIO_BUTTON,
    ITEM_CHECKBOX,
    ITEM_MESSAGE
} EasyUIItem_e;

typedef enum
{
    PAGE_LIST,
    PAGE_ICON,
    PAGE_CUSTOM
} EasyUIPage_e;

typedef struct EasyUI_item
{
    struct EasyUI_item *next;

    EasyUIItem_e funcType;
    uint8_t id;
    int16_t lineId;
    float posForCal;
    float step;
    int16_t position;
    char *title;

    char *msg;                                  // ITEM_MESSAGE
    bool *flag;                                 // ITEM_CHECKBOX and ITEM_RADIO_BUTTON and ITEM_SWITCH
    bool flagDefault;                           // Factory default setting
    paramType *param;                           // ITEM_CHANGE_VALUE and ITEM_PROGRESS_BAR
    paramType paramDefault;                     // Factory default setting
    paramType paramBackup;                      // ITEM_CHANGE_VALUE and ITEM_PROGRESS_BAR
    uint8_t pageId;                             // ITEM_JUMP_PAGE
    void (*Event)(struct EasyUI_item *item);    // ITEM_CHANGE_VALUE and ITEM_PROGRESS_BAR
} EasyUIItem_t;

typedef struct EasyUI_page
{
    struct EasyUI_page *next;

    EasyUIPage_e funcType;
    EasyUIItem_t *itemHead, *itemTail;
    uint8_t id;

    void (*Event)(struct EasyUI_page *page);
} EasyUIPage_t;

extern char *EasyUIVersion;
extern bool functionIsRunning, listLoop, errorOccurred, batteryMonitor;
extern EasyUIPage_t *pageHead, *pageTail;

void EasyUIAddItem(EasyUIPage_t *page, EasyUIItem_t *item, char *_title, EasyUIItem_e func, ...);
void EasyUIAddPage(EasyUIPage_t *page, EasyUIPage_e func, ...);
void EasyUITransitionAnim();
void EasyUIBackgroundBlur();
void EasyUIKeyActionMonitor();

void EasyUIDrawMsgBox(char *msg);
float EasyUIGetBatteryVoltage();

void EasyUIEventChangeUint(EasyUIItem_t *item);
void EasyUIEventChangeInt(EasyUIItem_t *item);
void EasyUIEventChangeFloat(EasyUIItem_t *item);
void EasyUIEventSaveSettings(EasyUIItem_t *item);
void EasyUIEventResetSettings(EasyUIItem_t *item);

void EasyUIInit(uint8_t mode);
void EasyUI(uint8_t timer);

#ifdef __cplusplus
}
#endif

#endif
