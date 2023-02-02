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
#define MAX_LAYER               10

// Represent the time it takes to play the animation, smaller the quicker. Unit: ms
#define INDICATOR_MOVE_TIME     50
#define ITEM_MOVE_TIME          50

#define EasyUIScreenInit()                                      (ips114_init())
#define EasyUIDisplayStr(x, y, str)                             (IPS114_ShowStr(x, y, str))
#define EasyUIDisplayFloat(x, y, dat, num, pointNum)            (IPS114_ShowFloat(x, y, dat, num, pointNum))
#define EasyUIDrawDot(x, y, color)                              (IPS114_DrawPoint(x, y, color))
#define EasyUIDrawBox(x, y, width, height, color)               (IPS114_DrawBox(x, y, width, height, color))
#define EasyUIDrawFrame(x, y, width, height, color)             (IPS114_DrawFrame(x, y, width, height, color))
#define EasyUIDrawRFrame(x, y, width, height, color, r)         (IPS114_DrawRFrame(x, y, width, height, color, r))
#define EasyUIDrawRBox(x, y, width, height, color, r)           (IPS114_DrawRBox(x, y, width, height, color, r))
#define EasyUIClearBuffer()                                     (IPS114_ClearBuffer())
#define EasyUISendBuffer()                                      (IPS114_SendBuffer())
#define EasyUISetDrawColor(mode)                                (IPS114_SetDrawColor(mode))
#define EasyUIDisplayBMP(x, y, width, height, pic)              (IPS114_ShowBMP(x, y, width, height, pic))
#define EasyUIModifyColor()                                     (IPS114_ModifyColor())

#ifdef FPU
typedef     double      paramType;
#else
typedef     int64_t     paramType;
#endif

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
extern bool functionIsRunning, listLoop;
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
