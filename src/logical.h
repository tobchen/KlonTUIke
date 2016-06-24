/*
 * logical.h
 *
 *  Created on: Jun 20, 2016
 *      Author: tobchen
 */

#ifndef KLONTUIKE_SRC_LOGICAL_H
#define KLONTUIKE_SRC_LOGICAL_H

#include <stdint.h>
#include <stdbool.h>

typedef struct klontuike_table KlonTUIke_Table;

KlonTUIke_Table* KlonTUIke_CreateTable();

void KlonTUIke_DestroyTable(KlonTUIke_Table* table);

void KlonTUIke_ResetupTable(KlonTUIke_Table* table);

void KlonTUIke_CursorUp(KlonTUIke_Table* table);
void KlonTUIke_CursorDown(KlonTUIke_Table* table);
void KlonTUIke_CursorLeft(KlonTUIke_Table* table);
void KlonTUIke_CursorRight(KlonTUIke_Table* table);

void KlonTUIke_CursorAction(KlonTUIke_Table* table);

void KlonTUIke_CancelSelection(KlonTUIke_Table* table);

bool KlonTUIke_SetCursor(KlonTUIke_Table* table, uint8_t cursor);

uint8_t KlonTUIke_GetFoundation(KlonTUIke_Table* table, uint8_t index);
uint8_t KlonTUIke_GetTableau(KlonTUIke_Table* table, uint8_t index,
		uint8_t position);
uint8_t KlonTUIke_GetOpenReserve(KlonTUIke_Table* table);
bool KlonTUIke_IsReserveLeft(KlonTUIke_Table* table);

uint8_t KlonTUIke_GetCursor(KlonTUIke_Table* table);
uint8_t KlonTUIke_GetSelection(KlonTUIke_Table* table);

void KlonTUIke_GetCardInfo(uint8_t card, uint8_t* suit, uint8_t* numeral);

bool KlonTUIke_HasWon(KlonTUIke_Table* table);

#endif /* KLONTUIKE_SRC_LOGICAL_H */
