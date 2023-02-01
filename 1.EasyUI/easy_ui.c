/*!
 * Copyright (c) 2023, ErBW_s
 * All rights reserved.
 * 
 * @author  Baohan
 */

#include "easy_ui.h"

char *EasyUIVersion = "v1.5b";
EasyUIPage_t *pageHead = NULL, *pageTail = NULL;

/*!
 * @brief   Add item to page
 *
 * @param   page        EasyUI page struct
 * @param   item        EasyUI item struct
 * @param   _title      String of item title
 * @param   func        See EasyUIItem_e
 * @param   ...         ITEM_PAGE_DESCRIPTION: ignore this
 *                      ITEM_CALL_FUNCTION: fill with function
 *                      ITEM_JUMP_PAGE: fill with target page id
 *                      ITEM_CHECKBOX / ITEM_RADIO_BUTTON / ITEM_SWITCH: fill with bool value
 *                      ITEM_CHANGE_VALUE / ITEM_PROGRESS_BAR: fill with param that need to be changed and matched function
 *                      ITEM_MESSAGE: fill with message and matched function
 * @return  void
 *
 * @note    Do not modify
 *          ITEM_CHANGE_VALUE: the incoming param should always be paramType *,
 *          and cannot use casted variables(Don't know why)
 *          ITEM_PROGRESS_BAR: the incoming param should be 0 - 100
 */
void EasyUIAddItem(EasyUIPage_t *page, EasyUIItem_t *item, char *_title, EasyUIItem_e func, ...)
{
    *item->flag = false;
    item->flagDefault = false;
    *item->param = 0;
    item->paramDefault = 0;
    item->paramBackup = 0;
    item->pageId = 0;
    item->Event = NULL;

    va_list variableArg;
    va_start(variableArg, func);
    item->title = _title;
    item->funcType = func;
    switch (item->funcType)
    {
        case ITEM_JUMP_PAGE:
            item->pageId = va_arg(variableArg, int);
            break;
        case ITEM_CHECKBOX:
        case ITEM_RADIO_BUTTON:
        case ITEM_SWITCH:
            item->flag = va_arg(variableArg, bool *);
            item->flagDefault = *item->flag;
            break;
        case ITEM_PROGRESS_BAR:
        case ITEM_CHANGE_VALUE:
            item->param = va_arg(variableArg, paramType *);
            item->paramBackup = *item->param;
            item->paramDefault = *item->param;
            item->Event = va_arg(variableArg, void (*)(EasyUIItem_t * ));
            break;
        case ITEM_MESSAGE:
            item->msg = va_arg(variableArg, char *);
            item->Event = va_arg(variableArg, void (*)(EasyUIItem_t * ));
        default:
            break;
    }
    va_end(variableArg);

    item->next = NULL;

    if (page->itemHead == NULL)
    {
        item->id = 0;
        page->itemHead = item;
        page->itemTail = item;
    } else
    {
        item->id = page->itemTail->id + 1;
        page->itemTail->next = item;
        page->itemTail = page->itemTail->next;
    }

    item->lineId = item->id;
    item->posForCal = 0;
    item->step = 0;
    item->position = 0;
}


/*!
 * @brief   Add page to UI
 *
 * @param   page    EasyUI page struct
 * @param   func    See EasyUIPage_e
 * @param   ...     PAGE_LIST: ignore this
 *                  PAGE_CUSTOM: fill with certain function
 * @return  void
 *
 * @note    Do not modify, the first page should always be the fist one to be added.
 */
void EasyUIAddPage(EasyUIPage_t *page, EasyUIPage_e func, ...)
{
    page->Event = NULL;

    va_list variableArg;
    va_start(variableArg, func);
    page->itemHead = NULL;
    page->itemTail = NULL;
    page->next = NULL;

    page->funcType = func;
    if (page->funcType == PAGE_CUSTOM)
        page->Event = va_arg(variableArg, void (*)(EasyUIPage_t * ));
    va_end(variableArg);

    if (pageHead == NULL)
    {
        page->id = 0;
        pageHead = page;
        pageTail = page;
    } else
    {
        page->id = pageTail->id + 1;
        pageTail->next = page;
        pageTail = pageTail->next;
    }
}


/*!
 * @brief   Blur transition animation
 *
 * @param   void
 * @return  void
 *
 * @note    Use before clearing the buffer
 *          Also use after all the initialization is done for better experience
 */
void EasyUITransitionAnim()
{
    for (int j = 1; j < SCREEN_HEIGHT + 1; j += 2)
    {
        for (int i = 0; i < SCREEN_WIDTH + 1; i += 2)
        {
            EasyUIDrawDot(i, j, IPS114_backgroundColor);
        }
    }
    EasyUISendBuffer();
    for (int j = 1; j < SCREEN_HEIGHT + 1; j += 2)
    {
        for (int i = 1; i < SCREEN_WIDTH + 1; i += 2)
        {
            EasyUIDrawDot(i, j, IPS114_backgroundColor);
        }
    }
    EasyUISendBuffer();
    for (int j = 0; j < SCREEN_HEIGHT + 1; j += 2)
    {
        for (int i = 1; i < SCREEN_WIDTH + 1; i += 2)
        {
            EasyUIDrawDot(i, j, IPS114_backgroundColor);
        }
    }
    EasyUISendBuffer();
    for (int j = 1; j < SCREEN_HEIGHT + 1; j += 2)
    {
        for (int i = 1; i < SCREEN_WIDTH + 1; i += 2)
        {
            EasyUIDrawDot(i - 1, j - 1, IPS114_backgroundColor);
        }
    }
    EasyUISendBuffer();
}


/*!
 * @brief   Blur the background for other use
 *
 * @param   void
 * @return  void
 */
void EasyUIBackgroundBlur()
{
    for (int j = 1; j < SCREEN_HEIGHT + 1; j += 2)
    {
        for (int i = 0; i < SCREEN_WIDTH + 1; i += 2)
        {
            EasyUIDrawDot(i, j, IPS114_backgroundColor);
        }
    }
    EasyUISendBuffer();
    for (int j = 1; j < SCREEN_HEIGHT + 1; j += 2)
    {
        for (int i = 1; i < SCREEN_WIDTH + 1; i += 2)
        {
            EasyUIDrawDot(i, j, IPS114_backgroundColor);
        }
    }
    EasyUISendBuffer();
    for (int j = 0; j < SCREEN_HEIGHT + 1; j += 2)
    {
        for (int i = 1; i < SCREEN_WIDTH + 1; i += 2)
        {
            EasyUIDrawDot(i, j, IPS114_backgroundColor);
        }
    }
    EasyUISendBuffer();
}


/*!
 * @brief   Draw message box
 *
 * @param   msg     The message need to be displayed
 * @return  void
 */
void EasyUIDrawMsgBox(char *msg)
{
    uint16_t width = strlen(msg) * FONT_WIDTH + 5;
    uint16_t x, y;
    uint8_t offset = 2;
    x = (SCREEN_WIDTH - width) / 2;
    y = (SCREEN_HEIGHT - ITEM_HEIGHT) / 2;
    EasyUIBackgroundBlur();
    EasyUIDrawRFrame(x + offset, y - offset, width, ITEM_HEIGHT, IPS114_penColor ,1);
    EasyUIDrawRBox(x - offset, y + offset, width, ITEM_HEIGHT, IPS114_penColor, 1);
    EasyUISetDrawColor(XOR);
    EasyUIDisplayStr(x - offset + 2, y + offset + (ITEM_HEIGHT - FONT_HEIGHT) / 2, msg);
    EasyUISetDrawColor(NORMAL);
}


/*!
 * @brief   Draw progress bar
 *
 * @param   item    EasyUI item struct
 * @return  void
 *
 * @note    Internal call
 */
void EasyUIDrawProgressBar(EasyUIItem_t *item)
{
    static int16_t x, y;
    static uint16_t width, height;
    static uint8_t itemHeightOffset = (ITEM_HEIGHT - FONT_HEIGHT) / 2 + 1;
    static uint16_t barWidth;

    EasyUISetDrawColor(NORMAL);

    // Display information and draw box
    height = ITEM_HEIGHT * 2 + 2;
    if (strlen(item->title) + 1 > 12)
        width = (strlen(item->title) + 1) * FONT_WIDTH + 7;
    else
        width = 12 * FONT_WIDTH + 7;
    if (width < 2 * SCREEN_WIDTH / 3)
        width = 2 * SCREEN_WIDTH / 3;
    x = (SCREEN_WIDTH - width) / 2;
    y = (SCREEN_HEIGHT - height) / 2;

    barWidth = width - 6 * FONT_WIDTH - 10;

    EasyUIDrawFrame(x - 1, y - 1, width + 2, height + 2, IPS114_penColor);
    EasyUIDrawBox(x, y, width, height, IPS114_backgroundColor);
    EasyUIDisplayStr(x + 3, y + itemHeightOffset, item->title);
    EasyUIDisplayStr(x + 3 + strlen(item->title) * FONT_WIDTH, y + itemHeightOffset, ":");
    EasyUIDrawFrame(x + 3, y + ITEM_HEIGHT + itemHeightOffset, barWidth, FONT_HEIGHT, IPS114_penColor);
    EasyUIDrawBox(x + 5, y + ITEM_HEIGHT + itemHeightOffset + 2, (float) *item->param / 100 * barWidth - 4, FONT_HEIGHT - 4, IPS114_penColor);
    EasyUIDisplayFloat(x + width - 6 * FONT_WIDTH - 4, y + ITEM_HEIGHT + itemHeightOffset, *item->param, 3, 2);

    EasyUISendBuffer();
}


/*!
 * @brief   Draw check box
 *
 * @param   x           Check box position x
 * @param   y           Check box position y
 * @param   size        Size of check box
 * @param   offset      Offset of selected rounded box
 * @param   boolValue   True of false
 * @return  void
 *
 * @note    Internal call
 */
void EasyUIDrawCheckbox(int16_t x, int16_t y, uint16_t size, uint8_t offset, bool boolValue, uint8_t r)
{
    IPS114_DrawRFrame(x, y, size, size, IPS114_penColor, r);
    if (boolValue)
        IPS114_DrawRBox(x + offset, y + offset, size - 2 * offset, size - 2 * offset, IPS114_penColor, r);
}


/*!
 * @brief   Get position of item with linear animation
 *
 * @param   page    Struct of page
 * @param   item    Struct of item
 * @param   index   Current index
 * @param   timer   Fill this with interrupt trigger time
 * @return  void
 *
 * @note    Internal call
 */
void EasyUIGetItemPos(EasyUIPage_t *page, EasyUIItem_t *item, uint8_t index, uint8_t timer)
{
    static uint8_t itemHeightOffset = (ITEM_HEIGHT - FONT_HEIGHT) / 2;
    static uint16_t time = 0;
    static int16_t move = 0, target = 0;
    static uint8_t lastIndex = 0, moveFlag = 0;
    uint8_t speed = ITEM_MOVE_TIME / timer;

    // Item need to move or not
    if (moveFlag == 0)
    {
        for (EasyUIItem_t *itemTmp = page->itemHead; itemTmp != NULL; itemTmp = itemTmp->next)
        {
            if (index == itemTmp->id && itemTmp->lineId < 0)
            {
                move = itemTmp->lineId;
                moveFlag = 1;
                break;
            } else if (index == itemTmp->id && itemTmp->lineId > ITEM_LINES - 1)
            {
                move = itemTmp->lineId - ITEM_LINES + 1;
                moveFlag = 1;
                break;
            }
        }
    }

    // Change the item lineId and get target position
    for (EasyUIItem_t *itemTmp = page->itemHead; itemTmp != NULL; itemTmp = itemTmp->next)
    {
        itemTmp->lineId -= move;
    }
    move = 0;
    moveFlag = 0;
    target = itemHeightOffset + item->lineId * ITEM_HEIGHT;

    // Calculate current position
    if (time == 0 || index != lastIndex)
    {
        item->step = ((float) target - (float) item->position) / (float) speed;
    }
    if (time >= ITEM_MOVE_TIME)
    {
        item->posForCal = target;
    } else
        item->posForCal += item->step;

    item->position = (int16_t) item->posForCal;
    lastIndex = index;

    // Time counter
    if (item->next == NULL)
    {
        if (target == item->position)
            time = 0;
        else
            time += timer;
    }
}


/*!
 * @brief   Get position of indicator and scroll bar with linear animation
 *
 * @param   page    Struct of page
 * @param   index   Current index
 * @param   timer   Fill this with interrupt trigger time
 * @param   status  Fill this with 1 to reset height
 * @return  void
 *
 * @note    Internal call
 */
void EasyUIDrawIndicator(EasyUIPage_t *page, uint8_t index, uint8_t timer, uint8_t status)
{
    static float stepLength = 0, stepY = 0, length = 0, y = SCREEN_HEIGHT;
    static uint16_t time = 0;
    static uint8_t lastIndex = 0;
    static uint16_t lengthTarget = 0, yTarget = 0;
    uint8_t speed = INDICATOR_MOVE_TIME / timer;

    if (status)
        y = SCREEN_HEIGHT;

    // Get Initial length
    if ((int) length == 0)
    {
        if (page->itemHead->funcType == ITEM_PAGE_DESCRIPTION)
            length = (float) (strlen(page->itemHead->title)) * FONT_WIDTH + 5;
        else
            length = (float) (strlen(page->itemHead->title) + 1) * FONT_WIDTH + 8;
    }

    // Get target length and y
    for (EasyUIItem_t *itemTmp = page->itemHead; itemTmp != NULL; itemTmp = itemTmp->next)
    {
        if (index == itemTmp->id)
        {
            if (itemTmp->funcType == ITEM_PAGE_DESCRIPTION)
                lengthTarget = (strlen(itemTmp->title)) * FONT_WIDTH + 5;
            else
                lengthTarget = (strlen(itemTmp->title) + 1) * FONT_WIDTH + 8;
            yTarget = itemTmp->lineId * ITEM_HEIGHT;
            if (index != lastIndex && abs(index - lastIndex) < page->itemTail->id)
            {
                if (itemTmp->position < 0)
                    y = (float)3 * ITEM_HEIGHT / 4;
                else if (itemTmp->position >= (ITEM_LINES) * ITEM_HEIGHT)
                    y = (ITEM_LINES - 2) * ITEM_HEIGHT + (float)ITEM_HEIGHT / 4;
            }
            break;
        }
    }

    // Calculate current position
    if (time == 0 || index != lastIndex)
    {
        stepLength = ((float) lengthTarget - (float) length) / (float) speed;
        stepY = ((float) yTarget - (float) y) / (float) speed;
    }
    if (time >= ITEM_MOVE_TIME)
    {
        length = lengthTarget;
        y = yTarget;
    } else
    {
        length += stepLength;
        y += stepY;
    }

    // Draw rounded box and scroll bar
    EasyUISetDrawColor(XOR);
    EasyUIDrawRBox(0, (int16_t) y, (int16_t) length, ITEM_HEIGHT, IPS114_penColor, 1);
    EasyUISetDrawColor(NORMAL);
    EasyUIDrawRBox(SCREEN_WIDTH - SCROLL_BAR_WIDTH, (int16_t) y, SCROLL_BAR_WIDTH, ITEM_HEIGHT, IPS114_penColor, 1);
    lastIndex = index;

    // Time counter
    if ((int) length == lengthTarget && (int) y == yTarget)
        time = 0;
    else
        time += timer;
}





/*!
 * @brief   Event: change variable's value
 *
 * @param   item    EasyUI item struct
 * @return  void
 */
void EasyUIEventChangeUint(EasyUIItem_t *item)
{
    static int16_t x, y;
    static uint16_t width, height;
    static uint8_t index = 1, step = 1;
    static uint8_t itemHeightOffset = (ITEM_HEIGHT - FONT_HEIGHT) / 2 + 1;
    static bool changeVal = false, changeStep = false;

    EasyUISetDrawColor(NORMAL);

    // Display information and draw box
    height = ITEM_HEIGHT * 4 + 2;
    if (strlen(item->title) + 1 > 12)
        width = (strlen(item->title) + 1) * FONT_WIDTH + 7;
    else
        width = 12 * FONT_WIDTH + 7;
    if (width < 2 * SCREEN_WIDTH / 3)
        width = 2 * SCREEN_WIDTH / 3;
    x = (SCREEN_WIDTH - width) / 2;
    y = (SCREEN_HEIGHT - height) / 2;
    EasyUIDrawFrame(x - 1, y - 1, width + 2, height + 2, IPS114_penColor);
    EasyUIDrawBox(x, y, width, height, IPS114_backgroundColor);
    EasyUIDisplayStr(x + 3, y + itemHeightOffset, item->title);
    EasyUIDisplayStr(x + 3 + strlen(item->title) * FONT_WIDTH, y + itemHeightOffset, ":");
    EasyUIDisplayStr(x + 3, y + 2 * ITEM_HEIGHT + itemHeightOffset, "Step:");
    EasyUIDisplayStr(x + 3, y + 3 * ITEM_HEIGHT + itemHeightOffset, "Save");
    EasyUIDisplayStr(x + width - 6 * FONT_WIDTH - 4, y + 3 * ITEM_HEIGHT + itemHeightOffset, "Return");

    // Change value of param or step
    if (changeVal)
    {
        EasyUISetDrawColor(XOR);
        EasyUIDrawBox(x + 2, y + 2, (strlen(item->title) + 1) * FONT_WIDTH + 3, ITEM_HEIGHT - 2, IPS114_penColor);
        EasyUISetDrawColor(NORMAL);
        if (opnUp)
            *item->param += step;
        if (opnDown)
        {
            if (*item->param - step >= 0)
                *item->param -= step;
            else
                *item->param = 0;
        }
    } else if (changeStep)
    {
        EasyUISetDrawColor(XOR);
        EasyUIDrawBox(x + 2, y + 2 + 2 * ITEM_HEIGHT, 5 * FONT_WIDTH + 3, ITEM_HEIGHT - 2, IPS114_penColor);
        EasyUISetDrawColor(NORMAL);
        if (opnUp)
        {
            if (step == 1)
                step = 10;
            else if (step == 10)
                step = 100;
            else
                step = 1;
        }
        if (opnDown)
        {
            if (step == 100)
                step = 10;
            else if (step == 10)
                step = 1;
            else
                step = 100;
        }
    } else
    {
        if (opnForward)
        {
            if (index < 4)
                index++;
            else
                index = 1;
        }
        if (opnBackward)
        {
            if (index > 1)
                index--;
            else
                index = 4;
        }
    }

    // Display step
    EasyUIDisplayFloat(x + 3, y + ITEM_HEIGHT + itemHeightOffset, *item->param, 8, 2);
    if (step == 1)
        EasyUIDisplayStr(x + 3 + 6 * FONT_WIDTH, y + 2 * ITEM_HEIGHT + itemHeightOffset, "+1");
    else if (step == 10)
        EasyUIDisplayStr(x + 3 + 6 * FONT_WIDTH, y + 2 * ITEM_HEIGHT + itemHeightOffset, "+10");
    else
        EasyUIDisplayStr(x + 3 + 6 * FONT_WIDTH, y + 2 * ITEM_HEIGHT + itemHeightOffset, "+100");

    // Draw indicator
    if (index == 1)
        EasyUIDrawRFrame(x + 1, y + 1, (strlen(item->title) + 1) * FONT_WIDTH + 5, ITEM_HEIGHT, IPS114_penColor, 1);
    else if (index == 2)
        EasyUIDrawRFrame(x + 1, y + 1 + 2 * ITEM_HEIGHT, 5 * FONT_WIDTH + 5, ITEM_HEIGHT, IPS114_penColor, 1);
    else if (index == 3)
        EasyUIDrawRFrame(x + 1, y + 1 + 3 * ITEM_HEIGHT, 4 * FONT_WIDTH + 5, ITEM_HEIGHT, IPS114_penColor, 1);
    else
        EasyUIDrawRFrame(x + width - 6 * FONT_WIDTH - 6, y + 1 + 3 * ITEM_HEIGHT, 6 * FONT_WIDTH + 5, ITEM_HEIGHT,
                         IPS114_penColor, 1);

    // Operation move reaction
    if (opnEnter)
    {
        if (index == 1)
            changeVal = true;
        else if (index == 2)
            changeStep = true;
        else if (index == 3)
        {
            item->paramBackup = *item->param;
            functionIsRunning = false;
            EasyUIBackgroundBlur();
            index = 1;
            step = 1;
        } else
        {
            *item->param = item->paramBackup;
            functionIsRunning = false;
            EasyUIBackgroundBlur();
            index = 1;
            step = 1;
        }
    }
    if (opnExit)
    {
        if (index == 1)
            changeVal = false;
        else if (index == 2)
            changeStep = false;
    }

    IPS114_SendBuffer();
}
void EasyUIEventChangeInt(EasyUIItem_t *item)
{
    static int16_t x, y;
    static uint16_t width, height;
    static uint8_t index = 1, step = 1;
    static uint8_t itemHeightOffset = (ITEM_HEIGHT - FONT_HEIGHT) / 2 + 1;
    static bool changeVal = false, changeStep = false;

    EasyUISetDrawColor(NORMAL);

    // Display information and draw box
    height = ITEM_HEIGHT * 4 + 2;
    if (strlen(item->title) + 1 > 12)
        width = (strlen(item->title) + 1) * FONT_WIDTH + 7;
    else
        width = 12 * FONT_WIDTH + 7;
    if (width < 2 * SCREEN_WIDTH / 3)
        width = 2 * SCREEN_WIDTH / 3;
    x = (SCREEN_WIDTH - width) / 2;
    y = (SCREEN_HEIGHT - height) / 2;
    EasyUIDrawFrame(x - 1, y - 1, width + 2, height + 2, IPS114_penColor);
    EasyUIDrawBox(x, y, width, height, IPS114_backgroundColor);
    EasyUIDisplayStr(x + 3, y + itemHeightOffset, item->title);
    EasyUIDisplayStr(x + 3 + strlen(item->title) * FONT_WIDTH, y + itemHeightOffset, ":");
    EasyUIDisplayStr(x + 3, y + 2 * ITEM_HEIGHT + itemHeightOffset, "Step:");
    EasyUIDisplayStr(x + 3, y + 3 * ITEM_HEIGHT + itemHeightOffset, "Save");
    EasyUIDisplayStr(x + width - 6 * FONT_WIDTH - 4, y + 3 * ITEM_HEIGHT + itemHeightOffset, "Return");

    // Change value of param or step
    if (changeVal)
    {
        EasyUISetDrawColor(XOR);
        EasyUIDrawBox(x + 2, y + 2, (strlen(item->title) + 1) * FONT_WIDTH + 3, ITEM_HEIGHT - 2, IPS114_penColor);
        EasyUISetDrawColor(NORMAL);
        if (opnUp)
            *item->param += step;
        if (opnDown)
            *item->param -= step;
    } else if (changeStep)
    {
        EasyUISetDrawColor(XOR);
        EasyUIDrawBox(x + 2, y + 2 + 2 * ITEM_HEIGHT, 5 * FONT_WIDTH + 3, ITEM_HEIGHT - 2, IPS114_penColor);
        EasyUISetDrawColor(NORMAL);
        if (opnUp)
        {
            if (step == 1)
                step = 10;
            else if (step == 10)
                step = 100;
            else
                step = 1;
        }
        if (opnDown)
        {
            if (step == 100)
                step = 10;
            else if (step == 10)
                step = 1;
            else
                step = 100;
        }
    } else
    {
        if (opnForward)
        {
            if (index < 4)
                index++;
            else
                index = 1;
        }
        if (opnBackward)
        {
            if (index > 1)
                index--;
            else
                index = 4;
        }
    }

    // Display step
    EasyUIDisplayFloat(x + 3, y + ITEM_HEIGHT + itemHeightOffset, *item->param, 8, 2);
    if (step == 1)
        EasyUIDisplayStr(x + 3 + 6 * FONT_WIDTH, y + 2 * ITEM_HEIGHT + itemHeightOffset, "+1");
    else if (step == 10)
        EasyUIDisplayStr(x + 3 + 6 * FONT_WIDTH, y + 2 * ITEM_HEIGHT + itemHeightOffset, "+10");
    else
        EasyUIDisplayStr(x + 3 + 6 * FONT_WIDTH, y + 2 * ITEM_HEIGHT + itemHeightOffset, "+100");

    // Draw indicator
    if (index == 1)
        EasyUIDrawRFrame(x + 1, y + 1, (strlen(item->title) + 1) * FONT_WIDTH + 5, ITEM_HEIGHT, IPS114_penColor, 1);
    else if (index == 2)
        EasyUIDrawRFrame(x + 1, y + 1 + 2 * ITEM_HEIGHT, 5 * FONT_WIDTH + 5, ITEM_HEIGHT, IPS114_penColor, 1);
    else if (index == 3)
        EasyUIDrawRFrame(x + 1, y + 1 + 3 * ITEM_HEIGHT, 4 * FONT_WIDTH + 5, ITEM_HEIGHT, IPS114_penColor, 1);
    else
        EasyUIDrawRFrame(x + width - 6 * FONT_WIDTH - 6, y + 1 + 3 * ITEM_HEIGHT, 6 * FONT_WIDTH + 5, ITEM_HEIGHT,
                         IPS114_penColor, 1);

    // Operation move reaction
    if (opnEnter)
    {
        if (index == 1)
            changeVal = true;
        else if (index == 2)
            changeStep = true;
        else if (index == 3)
        {
            item->paramBackup = *item->param;
            functionIsRunning = false;
            EasyUIBackgroundBlur();
            index = 1;
            step = 1;
        } else
        {
            *item->param = item->paramBackup;
            functionIsRunning = false;
            EasyUIBackgroundBlur();
            index = 1;
            step = 1;
        }
    }
    if (opnExit)
    {
        if (index == 1)
            changeVal = false;
        else if (index == 2)
            changeStep = false;
    }

    IPS114_SendBuffer();
}
void EasyUIEventChangeFloat(EasyUIItem_t *item)
{
    static int16_t x, y;
    static uint16_t width, height;
    static uint8_t index = 1;
    static double step = 0.01;
    static uint8_t itemHeightOffset = (ITEM_HEIGHT - FONT_HEIGHT) / 2 + 1;
    static bool changeVal = false, changeStep = false;

    EasyUISetDrawColor(NORMAL);

    // Display information and draw box
    height = ITEM_HEIGHT * 4 + 2;
    if (strlen(item->title) + 1 > 12)
        width = (strlen(item->title) + 1) * FONT_WIDTH + 7;
    else
        width = 12 * FONT_WIDTH + 7;
    if (width < 2 * SCREEN_WIDTH / 3)
        width = 2 * SCREEN_WIDTH / 3;
    x = (SCREEN_WIDTH - width) / 2;
    y = (SCREEN_HEIGHT - height) / 2;
    EasyUIDrawFrame(x - 1, y - 1, width + 2, height + 2, IPS114_penColor);
    EasyUIDrawBox(x, y, width, height, IPS114_backgroundColor);
    EasyUIDisplayStr(x + 3, y + itemHeightOffset, item->title);
    EasyUIDisplayStr(x + 3 + strlen(item->title) * FONT_WIDTH, y + itemHeightOffset, ":");
    EasyUIDisplayStr(x + 3, y + 2 * ITEM_HEIGHT + itemHeightOffset, "Step:");
    EasyUIDisplayStr(x + 3, y + 3 * ITEM_HEIGHT + itemHeightOffset, "Save");
    EasyUIDisplayStr(x + width - 6 * FONT_WIDTH - 4, y + 3 * ITEM_HEIGHT + itemHeightOffset, "Return");

    // Change value of param or step
    if (changeVal)
    {
        EasyUISetDrawColor(XOR);
        EasyUIDrawBox(x + 2, y + 2, (strlen(item->title) + 1) * FONT_WIDTH + 3, ITEM_HEIGHT - 2, IPS114_penColor);
        EasyUISetDrawColor(NORMAL);
        if (opnUp)
            *item->param += step;
        if (opnDown)
            *item->param -= step;
    } else if (changeStep)
    {
        EasyUISetDrawColor(XOR);
        EasyUIDrawBox(x + 2, y + 2 + 2 * ITEM_HEIGHT, 5 * FONT_WIDTH + 3, ITEM_HEIGHT - 2, IPS114_penColor);
        EasyUISetDrawColor(NORMAL);
        if (opnUp)
        {
            if (step == 0.01)
                step = 0.1;
            else if (step == 0.1)
                step = 1;
            else
                step = 0.01;
        }
        if (opnDown)
        {
            if (step == 0.01)
                step = 1;
            else if (step == 1)
                step = 0.1;
            else
                step = 0.01;
        }
    } else
    {
        if (opnForward)
        {
            if (index < 4)
                index++;
            else
                index = 1;
        }
        if (opnBackward)
        {
            if (index > 1)
                index--;
            else
                index = 4;
        }
    }

    // Display step
    EasyUIDisplayFloat(x + 3, y + ITEM_HEIGHT + itemHeightOffset, *item->param, 8, 2);
    if (step == 0.01)
        EasyUIDisplayStr(x + 3 + 6 * FONT_WIDTH, y + 2 * ITEM_HEIGHT + itemHeightOffset, "+0.01");
    else if (step == 0.1)
        EasyUIDisplayStr(x + 3 + 6 * FONT_WIDTH, y + 2 * ITEM_HEIGHT + itemHeightOffset, "+0.1");
    else
        EasyUIDisplayStr(x + 3 + 6 * FONT_WIDTH, y + 2 * ITEM_HEIGHT + itemHeightOffset, "+1");

    // Draw indicator
    if (index == 1)
        EasyUIDrawRFrame(x + 1, y + 1, (strlen(item->title) + 1) * FONT_WIDTH + 5, ITEM_HEIGHT, IPS114_penColor, 1);
    else if (index == 2)
        EasyUIDrawRFrame(x + 1, y + 1 + 2 * ITEM_HEIGHT, 5 * FONT_WIDTH + 5, ITEM_HEIGHT, IPS114_penColor, 1);
    else if (index == 3)
        EasyUIDrawRFrame(x + 1, y + 1 + 3 * ITEM_HEIGHT, 4 * FONT_WIDTH + 5, ITEM_HEIGHT, IPS114_penColor, 1);
    else
        EasyUIDrawRFrame(x + width - 6 * FONT_WIDTH - 6, y + 1 + 3 * ITEM_HEIGHT, 6 * FONT_WIDTH + 5, ITEM_HEIGHT,
                         IPS114_penColor, 1);

    // Operation move reaction
    if (opnEnter)
    {
        if (index == 1)
            changeVal = true;
        else if (index == 2)
            changeStep = true;
        else if (index == 3)
        {
            item->paramBackup = *item->param;
            functionIsRunning = false;
            EasyUIBackgroundBlur();
            index = 1;
            step = 0.01;
        } else
        {
            *item->param = item->paramBackup;
            functionIsRunning = false;
            EasyUIBackgroundBlur();
            index = 1;
            step = 0.01;
        }
    }
    if (opnExit)
    {
        if (index == 1)
            changeVal = false;
        else if (index == 2)
            changeStep = false;
    }

    IPS114_SendBuffer();
}


/*!
 * @brief   Event: Save and reset settings in flash
 *
 * @param   item    Useless param, just be there to meet the function requirement;
 */
void EasyUIEventSaveSettings(EasyUIItem_t *item)
{
    static uint16_t bufIndex = 0;      // Flash buffer array index(0-255)
    static uint8_t secIndex = 63;     // Flash section index(63-)
    static uint8_t pageIndex = 3;      // Flash page index(3-0)
    uint32_t arr[2];

    for (EasyUIPage_t *page = pageHead; page != NULL; page = page->next)
    {
        for (EasyUIItem_t *itemTmp = page->itemHead; itemTmp != NULL; itemTmp = itemTmp->next)
        {
            switch (itemTmp->funcType)
            {
                case ITEM_CHECKBOX:
                case ITEM_RADIO_BUTTON:
                case ITEM_SWITCH:
                    if (bufIndex < 256)
                    {
                        flash_union_buffer[bufIndex].uint32_type = *itemTmp->flag;
                        bufIndex++;
                    } else if (pageIndex > 0)
                    {
                        // Change page automatically
                        flash_write_page_from_buffer(secIndex, pageIndex);
                        flash_buffer_clear();
                        pageIndex--;
                        bufIndex = 0;
                    } else
                    {
                        // Change section automatically
                        flash_write_page_from_buffer(secIndex, pageIndex);
                        flash_buffer_clear();
                        pageIndex = 3;
                        bufIndex = 0;
                        secIndex--;
                    }
                    break;
                case ITEM_PROGRESS_BAR:
                case ITEM_CHANGE_VALUE:
                    if (bufIndex < 256)
                    {
                        DoubleToInt(*itemTmp->param, arr);
                        flash_union_buffer[bufIndex].uint32_type = arr[0];
                        flash_union_buffer[++bufIndex].uint32_type = arr[1];
                        bufIndex++;
                    } else if (pageIndex > 0)
                    {
                        // Change page automatically
                        flash_write_page_from_buffer(secIndex, pageIndex);
                        flash_buffer_clear();
                        pageIndex--;
                        bufIndex = 0;
                    } else
                    {
                        // Change section automatically
                        flash_write_page_from_buffer(secIndex, pageIndex);
                        flash_buffer_clear();
                        pageIndex = 3;
                        bufIndex = 0;
                        secIndex--;
                    }
                    break;
                default:
                    break;
            }
        }
    }
    flash_write_page_from_buffer(secIndex, pageIndex);
    functionIsRunning = false;
    EasyUIBackgroundBlur();
}
void EasyUIEventResetSettings(EasyUIItem_t *item)
{
    for (EasyUIPage_t *page = pageHead; page != NULL; page = page->next)
    {
        for (EasyUIItem_t *itemTmp = page->itemHead; itemTmp != NULL; itemTmp = itemTmp->next)
        {
            switch (itemTmp->funcType)
            {
                case ITEM_CHECKBOX:
                case ITEM_RADIO_BUTTON:
                case ITEM_SWITCH:
                    *itemTmp->flag = itemTmp->flagDefault;
                    break;
                case ITEM_PROGRESS_BAR:
                case ITEM_CHANGE_VALUE:
                    *itemTmp->param = itemTmp->paramDefault;
                default:
                    break;
            }
        }
    }
    functionIsRunning = false;
    EasyUIBackgroundBlur();
}


EasyKey_t keyUp, keyDown, keyForward, keyBackward, keyConfirm;
uint8_t opnForward, opnBackward;
uint8_t opnEnter, opnExit, opnUp, opnDown;

/*!
 * @brief   Welcome Page with two size of photo, and read params from flash if not empty
 *
 * @param   mode    choose the size of photo (0 for smaller one and 1 for bigger one)
 * @return  void
 */
void EasyUIInit(uint8_t mode)
{
    EasyUIScreenInit();

    // Key init
    EasyKeyInit(&keyUp, KEY_UP);
    EasyKeyInit(&keyDown, KEY_DOWN);
    EasyKeyInit(&keyForward, KEY_FORWARD);
    EasyKeyInit(&keyBackward, KEY_BACKWARD);
#if KEY_NUM > 2
    EasyKeyInit(&keyConfirm, KEY_CONFIRM);
#endif

    // Power-off storage
    static uint16_t bufIndex = 0;      // Flash buffer array index(0-255)
    static uint8_t secIndex = 63;     // Flash section index(63-)
    static uint8_t pageIndex = 3;      // Flash page index(3-0)
    uint32_t arr[2];
    if (flash_check(secIndex, pageIndex))
    {
        for (EasyUIPage_t *page = pageHead; page != NULL; page = page->next)
        {
            for (EasyUIItem_t *itemTmp = page->itemHead; itemTmp != NULL; itemTmp = itemTmp->next)
            {
                flash_read_page_to_buffer(secIndex, pageIndex);
                switch (itemTmp->funcType)
                {
                    case ITEM_CHECKBOX:
                    case ITEM_RADIO_BUTTON:
                    case ITEM_SWITCH:
                        if (bufIndex < 256)
                        {
                            *itemTmp->flag = flash_union_buffer[bufIndex].uint32_type;
                            bufIndex++;
                        } else if (pageIndex > 0)
                        {
                            // Change page automatically
                            pageIndex--;
                            bufIndex = 0;
                        } else
                        {
                            // Change section automatically
                            pageIndex = 3;
                            bufIndex = 0;
                            secIndex--;
                        }
                        break;
                    case ITEM_PROGRESS_BAR:
                    case ITEM_CHANGE_VALUE:
                        if (bufIndex < 256)
                        {
                            arr[0] = flash_union_buffer[bufIndex].uint32_type;
                            arr[1] = flash_union_buffer[++bufIndex].uint32_type;
                            IntToDouble(itemTmp->param, arr);
                            bufIndex++;
                        } else if (pageIndex > 0)
                        {
                            // Change page automatically
                            pageIndex--;
                            bufIndex = 0;
                        } else
                        {
                            // Change section automatically
                            pageIndex = 3;
                            bufIndex = 0;
                            secIndex--;
                        }
                        break;
                    default:
                        break;
                }
            }
        }
    }

    // Display the welcome photo and info
    EasyUIModifyColor();
    EasyUIClearBuffer();
    if (mode)
        EasyUIDisplayBMP((SCREEN_WIDTH - 58) / 2, (SCREEN_HEIGHT - 56) / 2, 58, 56, ErBW_s_5856);
    else
        EasyUIDisplayBMP((SCREEN_WIDTH - 29) / 2, (SCREEN_HEIGHT - 28) / 2, 29, 28, ErBW_s_2928);
    if (SCREEN_WIDTH > (25 * FONT_WIDTH + 1))
        EasyUIDisplayStr(SCREEN_WIDTH - 1 - 25 * FONT_WIDTH, SCREEN_HEIGHT - 1 - FONT_HEIGHT,
                         "Powered by EasyUI(ErBW_s)");
    else if (SCREEN_WIDTH > (14 * FONT_WIDTH + 1))
        EasyUIDisplayStr(SCREEN_WIDTH - 1 - 25 * FONT_WIDTH, SCREEN_HEIGHT - 1 - FONT_HEIGHT, "EasyUI(ErBW_s)");
    EasyUISendBuffer();
}


/*!
 * @brief   Sync the operation bool value
 *
 * @param   void
 * @return  void
 */
void EasyUISyncOpnValue()
{
#if KEY_NUM == 2
    opnForward = keyForward.isPressed;
    opnBackward = keyBackward.isPressed;
    opnEnter = keyForward.isHold;
    opnExit = keyBackward.isHold;
    opnUp = keyUp.isPressed;
    opnDown = keyDown.isPressed;
#elif KEY_NUM > 2
    opnForward = keyForward.isPressed;
    opnBackward = keyBackward.isPressed;
    opnEnter = keyConfirm.isPressed;
    opnExit = keyConfirm.isHold;
    opnUp = keyUp.isPressed;
    opnDown = keyDown.isPressed;
#endif

#if ROTARY == 1
#endif
}


bool functionIsRunning = false, listLoop = true;

/*!
 * @brief   Main function of EasyUI
 *
 * @param   timer   Fill this with interrupt trigger time
 * @return  void
 */
void EasyUI(uint8_t timer)
{
    static uint8_t pageIndex[10] = {0};     // Page id (stack)
    static uint8_t itemIndex[10] = {0};     // Item id (stack)
    static uint8_t layer = 0;               // pageIndex[layer] / itemIndex[layer]
    static uint8_t index = 0, itemSum = 0;

    EasyUISyncOpnValue();
    EasyUIModifyColor();
    EasyUISetDrawColor(NORMAL);

    // Get current page by id
    EasyUIPage_t *page = pageHead;
    while (page->id != pageIndex[layer])
    {
        page = page->next;
    }

    // Quit UI to run function
    // If running function and hold the confirm button, quit the function
    if (functionIsRunning)
    {
        for (EasyUIItem_t *item = page->itemHead; item != NULL; item = item->next)
        {
            if (item->id == index)
            {
                switch (item->funcType)
                {
                    case ITEM_PROGRESS_BAR:
                        EasyUIDrawProgressBar(item);
                        item->Event(item);
                        break;
                    default:
                        item->Event(item);
                        break;
                }
                break;
            }
        }
        return;
    }

    EasyUIClearBuffer();

    if (page->funcType == PAGE_LIST)
    {
        // Display every item in current page
        for (EasyUIItem_t *item = page->itemHead; item != NULL; item = item->next)
        {
            EasyUIGetItemPos(page, item, index, timer);
            switch (item->funcType)
            {
                case ITEM_JUMP_PAGE:
                    EasyUIDisplayStr(2, item->position, "+");
                    EasyUIDisplayStr(5 + FONT_WIDTH, item->position, item->title);
                    break;
                case ITEM_PAGE_DESCRIPTION:
                    EasyUIDisplayStr(2, item->position, item->title);
                    break;
                case ITEM_CHECKBOX:
                case ITEM_RADIO_BUTTON:
                    EasyUIDisplayStr(2, item->position, "-");
                    EasyUIDisplayStr(5 + FONT_WIDTH, item->position, item->title);
                    EasyUIDrawCheckbox(SCREEN_WIDTH - 7 - SCROLL_BAR_WIDTH - ITEM_HEIGHT + 2,
                                       item->position - (ITEM_HEIGHT - FONT_HEIGHT) / 2 + 1, ITEM_HEIGHT - 2, 3,
                                       *item->flag, 1);
                    break;
                case ITEM_SWITCH:
                    EasyUIDisplayStr(2, item->position, "-");
                    EasyUIDisplayStr(5 + FONT_WIDTH, item->position, item->title);
                    if (*item->flag == false)
                        EasyUIDisplayStr(SCREEN_WIDTH - 7 - 3 * FONT_WIDTH - SCROLL_BAR_WIDTH, item->position, "off");
                    else
                        EasyUIDisplayStr(SCREEN_WIDTH - 7 - 2 * FONT_WIDTH - SCROLL_BAR_WIDTH, item->position, "on");
                    break;
                case ITEM_PROGRESS_BAR:
                case ITEM_CHANGE_VALUE:
                    EasyUIDisplayStr(2, item->position, "-");
                    EasyUIDisplayStr(5 + FONT_WIDTH, item->position, item->title);
                    if (*item->param < 10 && *item->param >= 0)
                        EasyUIDisplayFloat(SCREEN_WIDTH - 7 - 4 * FONT_WIDTH - SCROLL_BAR_WIDTH, item->position,
                                           *item->param, 4, 2);
                    else if (*item->param < 100 && *item->param > -10)
                        EasyUIDisplayFloat(SCREEN_WIDTH - 7 - 5 * FONT_WIDTH - SCROLL_BAR_WIDTH, item->position,
                                           *item->param, 4, 2);
                    else if (*item->param < 1000 && *item->param > -100)
                        EasyUIDisplayFloat(SCREEN_WIDTH - 7 - 6 * FONT_WIDTH - SCROLL_BAR_WIDTH, item->position,
                                           *item->param, 4, 2);
                    else if (*item->param < 10000 && *item->param > -1000)
                        EasyUIDisplayFloat(SCREEN_WIDTH - 7 - 7 * FONT_WIDTH - SCROLL_BAR_WIDTH, item->position,
                                           *item->param, 4, 2);
                    else    // Hide because it's too long
                        EasyUIDisplayStr(SCREEN_WIDTH - 7 - 5 * FONT_WIDTH - SCROLL_BAR_WIDTH, item->position, "**.**");
                    break;
                default:
                    EasyUIDisplayStr(2, item->position, "-");
                    EasyUIDisplayStr(5 + FONT_WIDTH, item->position, item->title);
                    break;
            }
        }
        // Draw indicator and scroll bar
        EasyUIDrawIndicator(page, index, timer, 0);

        // Operation move reaction
        itemSum = page->itemTail->id;
        if (opnForward)
        {
            if (index < itemSum)
                index++;
            else if (listLoop)
                index = 0;
        }
        if (opnBackward)
        {
            if (index > 0)
                index--;
            else if (listLoop)
                index = itemSum;
        }
        if (opnEnter)
        {
            for (EasyUIItem_t *item = page->itemHead; item != NULL; item = item->next)
            {
                if (item->id == index)
                {
                    switch (item->funcType)
                    {
                        case ITEM_JUMP_PAGE:
                            if (layer < 9)
                            {
                                pageIndex[layer] = page->id;
                                itemIndex[layer] = index;
                                layer++;
                                pageIndex[layer] = item->pageId;
                                index = 0;
                                for (EasyUIItem_t *itemTmp = page->itemHead; itemTmp != NULL; itemTmp = itemTmp->next)
                                {
                                    itemTmp->position = 0;
                                    itemTmp->posForCal = 0;
                                }
                                EasyUITransitionAnim();
                                break;
                            } else
                                break;
                        case ITEM_CHECKBOX:
                        case ITEM_SWITCH:
                            *item->flag = !*item->flag;
                            break;
                        case ITEM_RADIO_BUTTON:
                            for (EasyUIItem_t *itemTmp = page->itemHead; itemTmp != NULL; itemTmp = itemTmp->next)
                            {
                                if (itemTmp->funcType == ITEM_RADIO_BUTTON && itemTmp->id != item->id)
                                    *itemTmp->flag = false;
                            }
                            *item->flag = !*item->flag;
                            break;
                        case ITEM_PROGRESS_BAR:
                        case ITEM_CHANGE_VALUE:
                            functionIsRunning = true;
                            EasyUIBackgroundBlur();
                            break;
                        case ITEM_MESSAGE:
                            functionIsRunning = true;
                            EasyUIDrawMsgBox(item->msg);
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        if (opnExit)
        {
            if (layer > 0)
            {
                pageIndex[layer] = 0;
                itemIndex[layer] = 0;
                layer--;
                index = itemIndex[layer];
                for (EasyUIItem_t *itemTmp = page->itemHead; itemTmp != NULL; itemTmp = itemTmp->next)
                {
                    itemTmp->position = 0;
                    itemTmp->posForCal = 0;
                }
                EasyUITransitionAnim();
            }
        }
    } else  // Run custom page
    {
        page->Event(page);
        if (opnExit)
        {
            if (layer > 0)
            {
                pageIndex[layer] = 0;
                itemIndex[layer] = 0;
                layer--;
                index = itemIndex[layer];
                EasyUITransitionAnim();
                EasyUIDrawIndicator(page, index, timer, 1);
            }
        }
    }

    EasyUISendBuffer();
}