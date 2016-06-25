/*
 * lv_interface.h
 *
 *  Created on: Jun 24, 2016
 *      Author: tobchen
 */

#ifndef KLONTUIKE_SRC_LV_INTERFACE_H
#define KLONTUIKE_SRC_LV_INTERFACE_H

#include <stdbool.h>

#include "logical.h"

typedef struct klontuike_lv_interface KlonTUIke_LVInterface;

KlonTUIke_LVInterface* KlonTUIke_CreateInterface();
void KlonTUIke_DestroyInterface(KlonTUIke_LVInterface* interface);

void KlonTUIke_ResetupInterface(KlonTUIke_LVInterface* interface);

void KlonTUIke_CursorUp(KlonTUIke_LVInterface* interface);
void KlonTUIke_CursorDown(KlonTUIke_LVInterface* interface);
void KlonTUIke_CursorLeft(KlonTUIke_LVInterface* interface);
void KlonTUIke_CursorRight(KlonTUIke_LVInterface* interface);

void KlonTUIke_CursorAction(KlonTUIke_LVInterface* interface);

void KlonTUIke_CancelSelection(KlonTUIke_LVInterface* interface);

bool KlonTUIke_SetCursor(KlonTUIke_LVInterface* interface, uint8_t cursor);

uint8_t KlonTUIke_GetCursor(KlonTUIke_LVInterface* interface);
uint8_t KlonTUIke_GetSelection(KlonTUIke_LVInterface* interface);

KlonTUIke_Table* KlonTUIke_GetTable(KlonTUIke_LVInterface* interface);

#endif /* KLONTUIKE_SRC_LV_INTERFACE_H */
