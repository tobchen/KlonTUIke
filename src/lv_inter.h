/*
 * lv_interface.h
 *
 *  Created on: Jun 24, 2016
 *      Author: tobchen
 */

#ifndef KTUI_SRC_LV_INTERFACE_H
#define KTUI_SRC_LV_INTERFACE_H

#include <stdbool.h>

#include "logical.h"

typedef struct ktui_lv_interface KTUI_LVInterface;

KTUI_LVInterface* KTUI_CreateInterface();
void KTUI_DestroyInterface(KTUI_LVInterface* interface);

void KTUI_ResetupInterface(KTUI_LVInterface* interface);

void KTUI_CursorUp(KTUI_LVInterface* interface);
void KTUI_CursorDown(KTUI_LVInterface* interface);
void KTUI_CursorLeft(KTUI_LVInterface* interface);
void KTUI_CursorRight(KTUI_LVInterface* interface);

void KTUI_CursorAction(KTUI_LVInterface* interface);

void KTUI_CancelSelection(KTUI_LVInterface* interface);

bool KTUI_SetCursor(KTUI_LVInterface* interface, uint8_t cursor);

uint8_t KTUI_GetCursor(KTUI_LVInterface* interface);
uint8_t KTUI_GetSelection(KTUI_LVInterface* interface);

KTUI_Table* KTUI_GetTable(KTUI_LVInterface* interface);

#endif /* KTUI_SRC_LV_INTERFACE_H */
