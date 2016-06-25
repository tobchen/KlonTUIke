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

void KlonTUIke_TurnReserve(KlonTUIke_Table* table);

bool KlonTUIke_FoundationToFoundation(KlonTUIke_Table* table,
		uint8_t indexFrom, uint8_t indexTo);
bool KlonTUIke_FoundationToTableau(KlonTUIke_Table* table,
		uint8_t indexFrom, uint8_t indexTo);

bool KlonTUIke_ReserveToFoundation(KlonTUIke_Table* table, uint8_t indexTo);
bool KlonTUIke_ReserveToTableau(KlonTUIke_Table* table, uint8_t indexTo);

bool KlonTUIke_TableauToFoundation(KlonTUIke_Table* table, uint8_t indexFrom,
		uint8_t indexTo);
bool KlonTUIke_TableauToTableau(KlonTUIke_Table* table,
		uint8_t indexFrom, uint8_t posFrom, uint8_t indexTo);

uint8_t KlonTUIke_GetTableau(KlonTUIke_Table* table, uint8_t index,
		uint8_t position);
uint8_t KlonTUIke_GetTableauSize(KlonTUIke_Table* table, uint8_t index);
uint8_t KlonTUIke_GetTableauFirstVis(KlonTUIke_Table* table, uint8_t index);

uint8_t KlonTUIke_GetFoundation(KlonTUIke_Table* table, uint8_t index);
uint8_t KlonTUIke_GetOpenReserve(KlonTUIke_Table* table);
bool KlonTUIke_IsReserveLeft(KlonTUIke_Table* table);

void KlonTUIke_GetCardInfo(uint8_t card, uint8_t* suit, uint8_t* numeral);

bool KlonTUIke_HasWon(KlonTUIke_Table* table);

#endif /* KLONTUIKE_SRC_LOGICAL_H */
