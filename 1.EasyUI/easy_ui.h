/*!
 * Copyright (c) 2023, ErBW_s
 * All rights reserved.
 * 
 * @author  Baohan
 */

#ifndef _EASY_UI_H
#define _EASY_UI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "easy_key.h"
#include "zf_device_ips114.h"
#include "zf_driver_uart.h"
#include "zf_driver_flash.h"

#include "user_flash.h"
#include "user_ips114.h"
#include "profile_photo_erbws.h"
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#include "menu.h"

// Modify this to fit your EasyKeyInit
#define KEY_UP          C7
#define KEY_DOWN        C6
#define KEY_FORWARD     C7
#define KEY_BACKWARD    C6
#define KEY_CONFIRM     G8

extern EasyKey_t keyUp, keyDown;           // Used to control value up and down
extern EasyKey_t keyForward, keyBackward;  // Used to control indicator movement
extern EasyKey_t keyConfirm;               // Used to change page or call function

// Operation response
extern uint8_t opnForward, opnBackward;
extern uint8_t opnEnter, opnExit, opnUp, opnDown;

#define KEY_NUM         3
#define ROTARY          0

#define SCREEN_WIDTH            240
#define SCREEN_HEIGHT           135
#define FONT_WIDTH              6
#define FONT_HEIGHT             8
#define ITEM_HEIGHT             16
#define SCROLL_BAR_WIDTH        4
#define ITEM_LINES              ((uint8_t)(SCREEN_HEIGHT / ITEM_HEIGHT))

// Represent the time it takes to play the animation, smaller the quicker. Unit: ms
#define INDICATOR_MOVE_TIME     60
#define ITEM_MOVE_TIME          60

#define EasyUIScreenInit()                                      (ips114_init())
#define EasyUIDisplayStr(x, y, str)                             (IPS114_ShowStr(x, y, str))
#define EasyUIDisplayFloat(x, y, dat, num, pointNum)            (IPS114_ShowFloat(x, y, dat, num, pointNum))
#define EasyUIDrawDot(x, y, color)                              (IPS114_DrawPoint(x, y, color))
#define EasyUIDrawBox(x, y, width, height, color)               (IPS114_DrawBox(x, y, width, height, color))
#define EasyUIDrawRFrame(x, y, width, height, color)            (IPS114_DrawRFrame(x, y, width, height, color))
#define EasyUIDrawRBox(x, y, width, height, color)              (IPS114_DrawRBox(x, y, width, height, color))
#define EasyUIDrawCheckbox(x, y, size, offset, value)           (IPS114_DrawCheckbox(x, y, size, offset, value))
#define EasyUIClearBuffer()                                     (IPS114_ClearBuffer())
#define EasyUISendBuffer()                                      (IPS114_SendBuffer())
#define EasyUISetDrawColor(mode)                                (IPS114_SetDrawColor(mode))
#define EasyUIDisplayBMP(x, y, width, height, pic)              (IPS114_ShowBMP(x, y, width, height, pic))
#define EasyUIModifyColor()                                     (IPS114_ModifyColor())

typedef enum
{
    ITEM_PAGE_DESCRIPTION,
    ITEM_JUMP_PAGE,
    ITEM_CALL_FUNCTION,
    ITEM_SWITCH,
    ITEM_CHANGE_VALUE,
    ITEM_RADIO_BUTTON,
    ITEM_CHECKBOX,
    ITEM_MESSAGE
} EasyUIItem_e;

typedef enum
{
    PAGE_LIST,
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
    double *param;                              // ITEM_CHANGE_VALUE
    double paramDefault;                        // Factory default setting
    double paramBackup;                         // ITEM_CHANGE_VALUE
    uint8_t pageId;                             // ITEM_JUMP_PAGE
    void (*Event)(struct EasyUI_item *item);    // ITEM_CALL_FUNCTION and ITEM_CHANGE_VALUE
} EasyUIItem_t;

typedef struct EasyUI_page
{
    struct EasyUI_page *next;

    EasyUIPage_e funcType;
    EasyUIItem_t *itemHead, *itemTail;
    uint8_t id;

    void (*Event)(struct EasyUI_page *page);
} EasyUIPage_t;

extern bool functionIsRunning;
extern EasyUIPage_t *pageHead, *pageTail;

void EasyUIAddItem(EasyUIPage_t *page, EasyUIItem_t *item, char *_title, EasyUIItem_e func, ...);
void EasyUIAddPage(EasyUIPage_t *page, EasyUIPage_e func, ...);
void EasyUITransitionAnim();
void EasyUIBackgroundBlur();
void EasyUISyncOpnValue();

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
