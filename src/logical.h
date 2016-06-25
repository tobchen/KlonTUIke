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

typedef struct ktui_table KTUI_Table;

KTUI_Table* KTUI_CreateTable();
void KTUI_DestroyTable(KTUI_Table* table);

void KTUI_ResetupTable(KTUI_Table* table);

void KTUI_TurnReserve(KTUI_Table* table);

bool KTUI_FoundationToFoundation(KTUI_Table* table,
		uint8_t indexFrom, uint8_t indexTo);
bool KTUI_FoundationToTableau(KTUI_Table* table,
		uint8_t indexFrom, uint8_t indexTo);

bool KTUI_ReserveToFoundation(KTUI_Table* table, uint8_t indexTo);
bool KTUI_ReserveToTableau(KTUI_Table* table, uint8_t indexTo);

bool KTUI_TableauToFoundation(KTUI_Table* table, uint8_t indexFrom,
		uint8_t indexTo);
bool KTUI_TableauToTableau(KTUI_Table* table,
		uint8_t indexFrom, uint8_t posFrom, uint8_t indexTo);

uint8_t KTUI_GetTableau(KTUI_Table* table, uint8_t index,
		uint8_t position);
uint8_t KTUI_GetTableauSize(KTUI_Table* table, uint8_t index);
uint8_t KTUI_GetTableauFirstVis(KTUI_Table* table, uint8_t index);

uint8_t KTUI_GetFoundation(KTUI_Table* table, uint8_t index);
uint8_t KTUI_GetOpenReserve(KTUI_Table* table);
bool KTUI_IsReserveLeft(KTUI_Table* table);

void KTUI_GetCardInfo(uint8_t card, uint8_t* suit, uint8_t* numeral);

bool KTUI_HasWon(KTUI_Table* table);

#endif /* KLONTUIKE_SRC_LOGICAL_H */
