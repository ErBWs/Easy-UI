/*!
 * Copyright (c) 2023, ErBW_s
 * All rights reserved.
 *
 * @author  Baohan
 */

#include "easyui_se.h"
#include "buzzer.h"

EasyUIPage_t *pageHead = NULL, *pageTail = NULL;

uint8_t pageIndex[MAX_LAYER] = {0};
uint8_t itemIndex[MAX_LAYER] = {0};
uint8_t layer = 0;

EasyKey_t keyUp, keyDown, keyForward, keyBackward, keyConfirm;
uint8_t opnForward, opnBackward;
uint8_t opnEnter, opnExit, opnUp, opnDown;

char *EasyUIVersion = "v1.5se";
bool functionIsRunning = false, listLoop = true, errorOccurred = false, batteryMonitor = true;

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
 *          If page type is PAGE_ICON, filled with icon array in the last variable
 */
void EasyUIAddItem(EasyUIPage_t *page, EasyUIItem_t *item, char *_title, EasyUIItem_e func, ...)
{
    if (item->flag) {
        *item->flag = false;
        item->flagDefault = false;
    }
    if (item->param) {
        *item->param = 0;
    }
    item->paramDefault = 0;
    item->paramBackup = 0;
    item->pageId = 0;
    item->Event = NULL;

    va_list variableArg;
    va_start(variableArg, func);
    item->title = _title;
    item->funcType = func;
    switch (item->funcType) {
    case ITEM_PAGE_DESCRIPTION:
        item->id = 0;
        break;
    case ITEM_JUMP_PAGE:
        item->id = 1;
        item->pageId = va_arg(variableArg, int);
        break;
    case ITEM_CHECKBOX:
    case ITEM_RADIO_BUTTON:
    case ITEM_SWITCH:
        item->id = 1;
        item->flag = va_arg(variableArg, bool *);
        item->flagDefault = *item->flag;
        break;
    case ITEM_PROGRESS_BAR:
    case ITEM_CHANGE_VALUE:
        item->id = 1;
        item->param = va_arg(variableArg, paramType *);
        item->paramBackup = *item->param;
        item->paramDefault = *item->param;
        item->Event = va_arg(variableArg, void (*)(EasyUIItem_t * ));
        break;
    case ITEM_MESSAGE:
        item->id = 1;
        item->msg = va_arg(variableArg, char *);
        item->Event = va_arg(variableArg, void (*)(EasyUIItem_t * ));
    default:
        break;
    }

    va_end(variableArg);

    item->next = NULL;

    if (page->itemHead == NULL) {
        page->itemHead = item;
        page->itemTail = item;
    } else {
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

    if (pageHead == NULL) {
        page->id = 0;
        pageHead = page;
        pageTail = page;
    } else {
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
    EasyUIClear();
}


/*!
 * @brief   Blur the background for other use
 *
 * @param   void
 * @return  void
 */
void EasyUIBackgroundBlur()
{
    EasyUIClear();
}


/*!
 * @brief   Draw message box
 *
 * @param   msg     The message need to be displayed
 * @return  void
 */
void EasyUIDrawMsgBox(char *msg)
{
    uint16_t width = strlen(msg) * FONT_WIDTH;
    EasyUIDisplayStr(SCREEN_WIDTH - width - 1, (ITEM_HEIGHT - FONT_HEIGHT) / 2, msg);
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
void EasyUIDrawCheckbox(int16_t x, int16_t y, bool boolValue)
{
    EasyUIDisplayStr(x, y, "[ ]");
    if (boolValue)
        EasyUIDisplayStr(x + FONT_WIDTH, y, "*");
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
void EasyUIGetItemPos(EasyUIPage_t *page, EasyUIItem_t *item, uint8_t index)
{
    static uint8_t itemHeightOffset = (ITEM_HEIGHT - FONT_HEIGHT) / 2;
    static uint16_t time = 0;
    static int16_t move = 0, target = 0;

    // Item need to move or not
    for (EasyUIItem_t *itemTmp = page->itemHead; itemTmp != NULL; itemTmp = itemTmp->next) {
        if (index == itemTmp->id && itemTmp->lineId < 1) {
            move = itemTmp->lineId;
            break;
        } else if (index == itemTmp->id && itemTmp->lineId > ITEM_LINES - 1) {
            move = itemTmp->lineId - ITEM_LINES + 1;
            break;
        }
    }

    // Change the item lineId and get target position
    if (move) {
        for (EasyUIItem_t *itemTmp = page->itemHead; itemTmp != NULL; itemTmp = itemTmp->next) {
            itemTmp->lineId -= move;
        }
        move = 0;
    }

    item->position = itemHeightOffset + item->lineId * ITEM_HEIGHT;
}


/*!
 * @brief   Display item according to its funcType
 * @param   item    Struct of item
 * @return  void
 *
 * @note    Internal call
 */
void EasyUIDisplayItem(EasyUIItem_t *item)
{
    if (item->funcType == ITEM_PAGE_DESCRIPTION) {
        EasyUIDisplayStr(0, item->position, item->title);
        return;
    }

    if (item->lineId<1 || item->lineId>ITEM_LINES - 1) {
        return;
    }

    switch (item->funcType) {
    case ITEM_JUMP_PAGE:
        EasyUIDisplayStr(0, item->position, "+");
        EasyUIDisplayStr(FONT_WIDTH + 1, item->position, item->title);
        break;
    case ITEM_CHECKBOX:
    case ITEM_RADIO_BUTTON:
        EasyUIDisplayStr(0, item->position, "-");
        EasyUIDisplayStr(FONT_WIDTH + 1, item->position, item->title);
        EasyUIDrawCheckbox(SCREEN_WIDTH - 3 * FONT_WIDTH - 1, item->position, *item->flag);
        break;
    case ITEM_SWITCH:
        EasyUIDisplayStr(0, item->position, "-");
        EasyUIDisplayStr(FONT_WIDTH + 1, item->position, item->title);
        if (*item->flag)
            EasyUIDisplayStr(SCREEN_WIDTH - 3 * FONT_WIDTH - 1, item->position, " on");
        else
            EasyUIDisplayStr(SCREEN_WIDTH - 3 * FONT_WIDTH - 1, item->position, "off");
        break;
    case ITEM_PROGRESS_BAR:
    case ITEM_CHANGE_VALUE:
        EasyUIDisplayStr(0, item->position, "-");
        EasyUIDisplayStr(FONT_WIDTH + 1, item->position, item->title);
        EasyUIDisplayFloat(SCREEN_WIDTH - 7 * FONT_WIDTH - 1, item->position, *item->param, 3, 2);
    default:
        EasyUIDisplayStr(0, item->position, "-");
        EasyUIDisplayStr(FONT_WIDTH + 1, item->position, item->title);
        break;
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
void EasyUIDrawIndicator(EasyUIPage_t *page, uint8_t index)
{
    if (index == 0)
        return;

    if (page->funcType != PAGE_LIST)
        return;

    // Get target length and y
    for (EasyUIItem_t *itemTmp = page->itemHead; itemTmp != NULL; itemTmp = itemTmp->next) {
        if (index == itemTmp->id) {
            EasyUIDisplayStr((strlen(itemTmp->title) + 1.5) * FONT_WIDTH, itemTmp->position, "<");
            break;
        }
    }
}


/*!
 * @brief   Different response to operation according to funcType
 *
 * @param   page    Struct of page
 * @param   item    Struct of item
 * @param   index   Current index
 * @return  void
 *
 * @note    Internal call
 */
void EasyUIItemOperationResponse(EasyUIPage_t *page, EasyUIItem_t *item, uint8_t *index)
{
    switch (item->funcType) {
    case ITEM_JUMP_PAGE:
        if (layer == MAX_LAYER - 1)
            break;

        itemIndex[layer++] = *index;
        pageIndex[layer] = item->pageId;
        *index = 1;
        EasyUIClear();
        break;
    case ITEM_CHECKBOX:
    case ITEM_SWITCH:
        *item->flag = !*item->flag;
        break;
    case ITEM_RADIO_BUTTON:
        for (EasyUIItem_t *itemTmp = page->itemHead; itemTmp != NULL; itemTmp = itemTmp->next) {
            if (itemTmp->funcType == ITEM_RADIO_BUTTON && itemTmp->id != item->id)
                *itemTmp->flag = false;
        }
        *item->flag = !*item->flag;
        EasyUIClear();
        break;
    case ITEM_PROGRESS_BAR:
    case ITEM_CHANGE_VALUE:
        functionIsRunning = true;
        break;
    case ITEM_MESSAGE:
        functionIsRunning = true;
        EasyUIDrawMsgBox(item->msg);
        break;
    default:
        break;
    }
}


/*!
 * @brief   Event: change variable's value
 *
 * @param   item    EasyUI item struct
 * @return  void
 */
void EasyUIEventChangeUint(EasyUIItem_t *item)
{
    static uint8_t step = 1;

    EasyUIDisplayStr(SCREEN_WIDTH - 8 * FONT_WIDTH - 1, item->position, ">");
    EasyUIDisplayStr(SCREEN_WIDTH - FONT_WIDTH - 1, item->position, "<");

    if (opnUp) {
        *item->param += step;
        EasyUIDisplayFloat(SCREEN_WIDTH - 7 * FONT_WIDTH - 1, item->position, *item->param, 3, 2);
    }
    if (opnDown) {
        if (*item->param - step >= 0)
            *item->param -= step;
        else
            *item->param = 0;
        EasyUIDisplayFloat(SCREEN_WIDTH - 7 * FONT_WIDTH - 1, item->position, *item->param, 3, 2);
    }
    if (opnEnter) {
        if (step == 1)
            step = 10;
        else if (step == 10)
            step = 100;
        else
            step = 1;
    }

    // Display step
    if (step == 1)
        EasyUIDisplayStr(SCREEN_WIDTH - 6 * FONT_WIDTH - 1, (ITEM_HEIGHT - FONT_HEIGHT) / 2, ":+1  ");
    else if (step == 10)
        EasyUIDisplayStr(SCREEN_WIDTH - 6 * FONT_WIDTH - 1, (ITEM_HEIGHT - FONT_HEIGHT) / 2, ":+10 ");
    else
        EasyUIDisplayStr(SCREEN_WIDTH - 6 * FONT_WIDTH - 1, (ITEM_HEIGHT - FONT_HEIGHT) / 2, ":+100");

    // Operation move reaction
    if (opnExit) {
        functionIsRunning = false;
        EasyUIClear();
    }

    // Clear the states of key to monitor next key action
    opnForward = opnBackward = opnEnter = opnExit = opnUp = opnDown = false;
}

void EasyUIEventChangeInt(EasyUIItem_t *item)
{
    static uint8_t step = 1;

    EasyUIDisplayStr(SCREEN_WIDTH - 8 * FONT_WIDTH - 1, item->position, ">");
    EasyUIDisplayStr(SCREEN_WIDTH - FONT_WIDTH - 1, item->position, "<");

    if (opnUp) {
        *item->param += step;
        EasyUIDisplayFloat(SCREEN_WIDTH - 7 * FONT_WIDTH - 1, item->position, *item->param, 3, 2);
    }
    if (opnDown) {
        *item->param -= step;
        EasyUIDisplayFloat(SCREEN_WIDTH - 7 * FONT_WIDTH - 1, item->position, *item->param, 3, 2);
    }
    if (opnEnter) {
        if (step == 1)
            step = 10;
        else if (step == 10)
            step = 100;
        else
            step = 1;
    }

    // Display step
    if (step == 1)
        EasyUIDisplayStr(SCREEN_WIDTH - 6 * FONT_WIDTH - 1, (ITEM_HEIGHT - FONT_HEIGHT) / 2, ":+1  ");
    else if (step == 10)
        EasyUIDisplayStr(SCREEN_WIDTH - 6 * FONT_WIDTH - 1, (ITEM_HEIGHT - FONT_HEIGHT) / 2, ":+10 ");
    else
        EasyUIDisplayStr(SCREEN_WIDTH - 6 * FONT_WIDTH - 1, (ITEM_HEIGHT - FONT_HEIGHT) / 2, ":+100");

    // Operation move reaction
    if (opnExit) {
        functionIsRunning = false;
        EasyUIClear();
    }

    // Clear the states of key to monitor next key action
    opnForward = opnBackward = opnEnter = opnExit = opnUp = opnDown = false;
}

void EasyUIEventChangeFloat(EasyUIItem_t *item)
{
    static float step = 0.01f;

    EasyUIDisplayStr(SCREEN_WIDTH - 8 * FONT_WIDTH - 1, item->position, ">");
    EasyUIDisplayStr(SCREEN_WIDTH - FONT_WIDTH - 1, item->position, "<");

    if (opnUp) {
        *item->param += step;
        EasyUIDisplayFloat(SCREEN_WIDTH - 7 * FONT_WIDTH - 1, item->position, *item->param, 3, 2);
    }
    if (opnDown) {
        *item->param -= step;
        EasyUIDisplayFloat(SCREEN_WIDTH - 7 * FONT_WIDTH - 1, item->position, *item->param, 3, 2);
    }
    if (opnEnter) {
        if (step == 0.01f)
            step = 0.1f;
        else if (step == 0.1f)
            step = 1.0f;
        else
            step = 0.01f;
    }

    // Display step
    if (step == 0.01f)
        EasyUIDisplayStr(SCREEN_WIDTH - 6 * FONT_WIDTH - 1, (ITEM_HEIGHT - FONT_HEIGHT) / 2, ":+0.01");
    else if (step == 0.1f)
        EasyUIDisplayStr(SCREEN_WIDTH - 6 * FONT_WIDTH - 1, (ITEM_HEIGHT - FONT_HEIGHT) / 2, ":+0.10");
    else
        EasyUIDisplayStr(SCREEN_WIDTH - 6 * FONT_WIDTH - 1, (ITEM_HEIGHT - FONT_HEIGHT) / 2, ":+1.00");

    // Operation move reaction
    if (opnExit) {
        functionIsRunning = false;
        EasyUIClear();
    }

    // Clear the states of key to monitor next key action
    opnForward = opnBackward = opnEnter = opnExit = opnUp = opnDown = false;
}


/*!
 * @brief   Event: Save and reset settings in flash
 *
 * @param   item    Useless param, just be there to meet the function requirement;
 */
void EasyUIEventSaveSettings(EasyUIItem_t *item)
{
    interrupt_global_disable();
    for (EasyUIPage_t *page = pageHead; page != NULL; page = page->next) {
        for (EasyUIItem_t *itemTmp = page->itemHead; itemTmp != NULL; itemTmp = itemTmp->next) {
            switch (itemTmp->funcType) {
            case ITEM_CHECKBOX:
            case ITEM_RADIO_BUTTON:
            case ITEM_SWITCH:
                SaveToFlash((int32_t *) itemTmp->flag);
                break;
            case ITEM_PROGRESS_BAR:
            case ITEM_CHANGE_VALUE:
                SaveToFlashWithConversion((double *) itemTmp->param);
                break;
            default:
                break;
            }
        }
    }
    FlashOperationEnd();
    interrupt_global_enable(1);
    functionIsRunning = false;
    EasyUIBackgroundBlur();
}

void EasyUIEventResetSettings(EasyUIItem_t *item)
{
    for (EasyUIPage_t *page = pageHead; page != NULL; page = page->next) {
        for (EasyUIItem_t *itemTmp = page->itemHead; itemTmp != NULL; itemTmp = itemTmp->next) {
            switch (itemTmp->funcType) {
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
    if (flash_check(flashSecIndex, flashPageIndex)) {
        interrupt_global_disable();
        for (EasyUIPage_t *page = pageHead; page != NULL; page = page->next) {
            for (EasyUIItem_t *itemTmp = page->itemHead; itemTmp != NULL; itemTmp = itemTmp->next) {
                switch (itemTmp->funcType) {
                case ITEM_CHECKBOX:
                case ITEM_RADIO_BUTTON:
                case ITEM_SWITCH:
                    ReadFlash((int32_t *) itemTmp->flag);
                    break;
                case ITEM_PROGRESS_BAR:
                case ITEM_CHANGE_VALUE:
                    ReadFlashWithConversion((double *) itemTmp->param);
                    break;
                default:
                    break;
                }
            }
        }
        FlashOperationEnd();
        interrupt_global_enable(1);
    }

    // Display the welcome info
    EasyUIDisplayStr(SCREEN_WIDTH / 2 - 8 * FONT_WIDTH - 1, SCREEN_HEIGHT / 2 - FONT_HEIGHT - 1, "EasyUISE(ErBW_s)");
}


/*!
 * @brief   Sync the operation bool value
 *
 * @param   void
 * @return  void
 */
void EasyUIKeyActionMonitor()
{
    if (opnForward || opnBackward || opnEnter || opnExit || opnUp || opnDown)
        return;

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


/*!
 * @brief   Main function of EasyUI
 *
 * @param   timer   Fill this with interrupt trigger time
 * @return  void
 */
void EasyUI(uint8_t timer)
{
    static uint8_t index = 1, itemSum = 0;

    // Get current page by id
    EasyUIPage_t *page = pageHead;
    while (page->id != pageIndex[layer]) {
        page = page->next;
    }

    // Quit UI to run function
    // If running function and hold the confirm button, quit the function
    if (functionIsRunning) {
        for (EasyUIItem_t *item = page->itemHead; item != NULL; item = item->next) {
            if (item->id != index) {
                continue;
            }
            item->Event(item);
            break;
        }
        return;
    }

//    EasyUIClear();

    // Custom page--------------------------------------------------------------------------------
    if (page->funcType == PAGE_CUSTOM) {
        page->Event(page);

        // Clear the states of key to monitor next key action
        opnForward = opnBackward = opnEnter = opnUp = opnDown = false;

        if (layer == 0) {
            opnExit = false;
            return;
        }

        if (opnExit) {
            opnExit = false;
            pageIndex[layer] = 0;
            itemIndex[layer--] = 0;
            index = itemIndex[layer];
            EasyUIDrawIndicator(page, index);
        }

        return;
    }
    // -------------------------------------------------------------------------------------------

    // List page----------------------------------------------------------------------------------
    for (EasyUIItem_t *item = page->itemHead; item != NULL; item = item->next) {
        EasyUIGetItemPos(page, item, index);
        EasyUIDisplayItem(item);
    }
    // Draw indicator and scroll bar
    EasyUIDrawIndicator(page, index);

    // Operation move reaction
    itemSum = page->itemTail->id;
    if (opnDown) {
        EasyUIClear();
        if (index < itemSum)
            index++;
        else if (listLoop)
            index = 1;
    }
    if (opnUp) {
        EasyUIClear();
        if (index > 1)
            index--;
        else if (listLoop)
            index = itemSum;
    }
    if (opnEnter) {
        for (EasyUIItem_t *item = page->itemHead; item != NULL; item = item->next) {
            if (item->id != index) {
                continue;
            }

            EasyUIItemOperationResponse(page, item, &index);
            break;
        }
    }

    // Clear the states of key to monitor next key action
    opnForward = opnBackward = opnEnter = opnUp = opnDown = false;

    if (layer == 0) {
        opnExit = false;
        return;
    }
    if (opnExit) {
        EasyUIClear();
        opnExit = false;
        pageIndex[layer] = 0;
        itemIndex[layer--] = 1;
        index = itemIndex[layer];
    }
    // -------------------------------------------------------------------------------------------
}