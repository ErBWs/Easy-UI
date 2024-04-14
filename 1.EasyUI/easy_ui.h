/*!
 * Copyright (c) 2023, ErBW_s
 * All rights reserved.
 *
 * @author  Baohan
 */

#ifndef _EASY_UI_H_
#define _EASY_UI_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "easy_key.h"
#include "zf_driver_uart.h"
#include "zf_driver_flash.h"
#include "zf_driver_adc.h"
#include "user_flash.h"
#include "ips096.h"
#include "profile_photo_erbws.h"
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

// Modify this to fit your EasyKeyInit
#define KEY_UP          E4
#define KEY_DOWN        E2
#define KEY_FORWARD     E4
#define KEY_BACKWARD    E2
#define KEY_CONFIRM     E3

extern EasyKey_t keyUp, keyDown;           // Used to control value up and down
extern EasyKey_t keyForward, keyBackward;  // Used to control indicator movement
extern EasyKey_t keyConfirm;               // Used to change page or call function

// Operation response
extern uint8_t opnForward, opnBackward;
extern uint8_t opnEnter, opnExit, opnUp, opnDown;

#define KEY_NUM         3
#define ROTARY          0

#define BATTERY_ADC_PIN         ADC1_IN9_B1
#define LOWEST_BATTERY_VOLTAGE  11.1f

#define SCREEN_WIDTH            160
#define SCREEN_HEIGHT           80
#define FONT_WIDTH              6
#define FONT_HEIGHT             8
#define ITEM_HEIGHT             12
#define CHECK_BOX_OFFSET        2
#define SCROLL_BAR_WIDTH        4
#define ITEM_LINES              ((uint8_t)(SCREEN_HEIGHT / ITEM_HEIGHT))
#define MAX_LAYER               10
#define ICON_SIZE               50

// Represent the time it takes to play the animation, smaller the quicker. Unit: ms
#define INDICATOR_MOVE_TIME     140
#define ITEM_MOVE_TIME          140
#define TRANSITION_TIME         120

#define EasyUIScreenInit()                                      (IPS096_Init())
#define EasyUIDisplayStr(x, y, str)                             (IPS096_ShowStr(x, y, str))
#define EasyUIDisplayFloat(x, y, dat, num, pointNum)            (IPS096_ShowFloat(x, y, dat, num, pointNum))
#define EasyUIDrawDot(x, y, color)                              (IPS096_DrawPoint(x, y, color))
#define EasyUIDrawBox(x, y, width, height, color)               (IPS096_DrawBox(x, y, width, height, color))
#define EasyUIDrawFrame(x, y, width, height, color)             (IPS096_DrawFrame(x, y, width, height, color))
#define EasyUIDrawRFrame(x, y, width, height, color, r)         (IPS096_DrawRFrame(x, y, width, height, color, r))
#define EasyUIDrawRBox(x, y, width, height, color, r)           (IPS096_DrawRBox(x, y, width, height, color, r))
#define EasyUIClearBuffer()                                     (IPS096_ClearBuffer())
#define EasyUISendBuffer()                                      (IPS096_SendBuffer())
#define EasyUISetDrawColor(mode)                                (IPS096_SetDrawColor(mode))
#define EasyUIDisplayBMP(x, y, width, height, pic)              (IPS096_ShowBMP(x, y, width, height, pic))
#define EasyUIModifyColor()                                     (IPS096_ModifyColor())

extern float GetBatteryVoltage();
#define EasyUIGetBatVoltage()                                   (GetBatteryVoltage())
#define EasyUIDelay_ms(time)                                    (system_delay_ms(time))

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

    uint8_t *icon;                              // PAGE_ICON
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
