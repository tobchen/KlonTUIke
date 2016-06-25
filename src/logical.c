/*
 * logical.c
 *
 *  Created on: Jun 20, 2016
 *      Author: tobchen
 */

#include "logical.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct tableau {
	uint8_t cards[19];
	uint8_t size;
	uint8_t firstVisible;
} Tableau;

typedef struct reserve {
	uint8_t cards[24];
	uint8_t size;
	uint8_t current;
} Reserve;

struct ktui_table {
	/* Staples */
	Tableau tableaus[7];
	Reserve reserve;

	/* Foundation
	 * Card >= 52 - empty foundation */
	uint8_t foundations[4];
};

static void removeFromFoundation(KTUI_Table* table, uint8_t index);
static void removeFromReserve(KTUI_Table* table);
static void removeFromTableau(KTUI_Table* table, uint8_t index,
		uint8_t first);

static void placeOnTableau(KTUI_Table* table, uint8_t index,
		uint8_t* cards, uint8_t length);

static bool mayBeOnFoundation(KTUI_Table* table, uint8_t index,
		uint8_t card);
static bool mayBeOnTableau(KTUI_Table* table, uint8_t index, uint8_t card);

KTUI_Table* KTUI_CreateTable() {
	KTUI_Table* table = malloc(sizeof(KTUI_Table));
	if (NULL == table) {
		return NULL;
	}

	KTUI_ResetupTable(table);

	return table;
}

void KTUI_DestroyTable(KTUI_Table* table) {
	if (table != NULL) {
		free(table);
	}
}

void KTUI_ResetupTable(KTUI_Table* table) {
	uint8_t deck[52];
	uint8_t i;
	uint8_t j;
	uint8_t random;
	uint8_t currentCard;

	if (NULL == table) {
		return;
	}

	/* Shuffle deck */
	for (i = 0; i < 52; i++) {
		deck[i] = i;
	}
	for (i = 51; i > 0; i--) {
		random = rand() % (i+1);
		j = deck[random];
		deck[random] = deck[i];
		deck[i] = j;
	}

	/* Foundations */
	for (i = 0; i < 4; i++) {
		table->foundations[i] = 52;
	}

	/* Now deal! */
	currentCard = 0;

	/* Tableaus */
	for (i = 0; i < 7; i++) {
		for (j = 0; j <= i; j++) {
			table->tableaus[i].cards[j] = deck[currentCard];
			currentCard++;
		}
		table->tableaus[i].size = i + 1;
		table->tableaus[i].firstVisible = i;
	}

	/* Reserve */
	for (i = 0; i < 24; i++) {
		table->reserve.cards[i] = deck[currentCard];
		currentCard++;
	}
	table->reserve.size = 24;
	table->reserve.current = 24;
}

void KTUI_TurnReserve(KTUI_Table* table) {
	if (table != NULL) {
		table->reserve.current++;
		if (table->reserve.current > table->reserve.size) {
			table->reserve.current = 0;
		}
	}
}

bool KTUI_FoundationToFoundation(KTUI_Table* table,
		uint8_t indexFrom, uint8_t indexTo) {
	/* TODO Check index bounds */
	if (table != NULL && indexFrom != indexTo
			&& mayBeOnFoundation(table, indexTo, table->foundations[indexFrom])) {
		table->foundations[indexTo] = table->foundations[indexFrom];
		removeFromFoundation(table, indexFrom);
		return true;
	}
	return false;
}

bool KTUI_FoundationToTableau(KTUI_Table* table,
		uint8_t indexFrom, uint8_t indexTo) {
	/* TODO Check index bounds */
	if (table != NULL &&
			mayBeOnTableau(table, indexTo, table->foundations[indexFrom])) {
		placeOnTableau(table, indexTo,
				table->foundations + indexFrom, 1);
		removeFromFoundation(table, indexFrom);
		return true;
	}
	return false;
}

bool KTUI_ReserveToFoundation(KTUI_Table* table, uint8_t indexTo) {
	uint8_t reserveCard;

	/* TODO Check index bounds */
	if (table != NULL) {
		reserveCard = KTUI_GetOpenReserve(table);
		if (mayBeOnFoundation(table, indexTo, reserveCard)) {
			table->foundations[indexTo] = reserveCard;
			removeFromReserve(table);
			return true;
		}
	}
	return false;
}

bool KTUI_ReserveToTableau(KTUI_Table* table, uint8_t indexTo) {
	uint8_t reserveCard;

	/* TODO Check index bounds */
	if (table != NULL) {
		reserveCard = KTUI_GetOpenReserve(table);
		if (mayBeOnTableau(table, indexTo, reserveCard)) {
			placeOnTableau(table, indexTo, &reserveCard, 1);
			removeFromReserve(table);
			return true;
		}
	}
	return false;
}

bool KTUI_TableauToFoundation(KTUI_Table* table, uint8_t indexFrom,
		uint8_t indexTo) {
	uint8_t tableauCard;
	uint8_t lastTableauPos;

	/* TODO Check index bounds */
	if (table != NULL && table->tableaus[indexFrom].size > 0) {
		lastTableauPos = table->tableaus[indexFrom].size - 1;
		tableauCard = table->tableaus[indexFrom].cards[lastTableauPos];
		if (mayBeOnFoundation(table, indexTo, tableauCard)) {
			table->foundations[indexTo] = tableauCard;
			removeFromTableau(table, indexFrom, lastTableauPos);
			return true;
		}
	}
	return false;
}

bool KTUI_TableauToTableau(KTUI_Table* table,
		uint8_t indexFrom, uint8_t posFrom, uint8_t indexTo) {
	uint8_t tableauCard;

	/* TODO Check index bounds */
	if (table != NULL) {
		tableauCard = KTUI_GetTableau(table, indexFrom, posFrom);
		if (indexFrom != indexTo
				&& mayBeOnTableau(table, indexTo, tableauCard)) {
			placeOnTableau(table, indexTo,
					table->tableaus[indexFrom].cards + posFrom,
					table->tableaus[indexFrom].size - posFrom);
			removeFromTableau(table, indexFrom, posFrom);
			return true;
		}
	}
	return false;
}

uint8_t KTUI_GetTableau(KTUI_Table* table, uint8_t index,
		uint8_t position) {
	if (NULL == table || index >= 7 || position >= table->tableaus[index].size) {
		return 52;
	} else if (position < table->tableaus[index].firstVisible) {
		return 53;
	} else {
		return table->tableaus[index].cards[position];
	}
}

uint8_t KTUI_GetTableauSize(KTUI_Table* table, uint8_t index) {
	if (table != NULL && index < 7) {
		return table->tableaus[index].size;
	} else {
		return 0;
	}
}

uint8_t KTUI_GetTableauFirstVis(KTUI_Table* table, uint8_t index) {
	if (table != NULL && index < 7) {
		return table->tableaus[index].firstVisible;
	} else {
		return 255;
	}
}

uint8_t KTUI_GetFoundation(KTUI_Table* table, uint8_t index) {
	if (NULL == table) {
		return 52;
	} else {
		return table->foundations[index];
	}
}

uint8_t KTUI_GetOpenReserve(KTUI_Table* table) {
	if (NULL == table || table->reserve.current >= table->reserve.size) {
		return 52;
	} else {
		return table->reserve.cards[table->reserve.current];
	}
}

bool KTUI_IsReserveLeft(KTUI_Table* table) {
	if (NULL == table || table->reserve.size == 0
			|| table->reserve.current + 1 == table->reserve.size) {
		return false;
	} else {
		return true;
	}
}

void KTUI_GetCardInfo(uint8_t card, uint8_t* suit, uint8_t* numeral) {
	if (suit != NULL) {
		*suit = card / 13;
	}
	if (numeral != NULL) {
		*numeral = card % 13;
	}
}

bool KTUI_HasWon(KTUI_Table* table) {
	uint8_t i;
	if (NULL == table) {
		return false;
	}
	for (i = 0; i < 4; i++) {
		if (table->foundations[i] >= 52 || table->foundations[i] % 13 != 12) {
			return false;
		}
	}
	return true;
}

static void removeFromFoundation(KTUI_Table* table, uint8_t index) {
	if (NULL == table || table->foundations[index] >= 52) {
		return;
	}
	/* Remove ace */
	if (table->foundations[index] % 13 == 0) {
		table->foundations[index] = 52;
	/* Remove 2 or bigger */
	} else {
		table->foundations[index]--;
	}
}

static void removeFromReserve(KTUI_Table* table) {
	if (NULL == table || table->reserve.current >= table->reserve.size) {
		return;
	}
	memmove(table->reserve.cards + table->reserve.current,
			table->reserve.cards + table->reserve.current + 1,
			table->reserve.size - table->reserve.current - 1);
	table->reserve.size--;
	table->reserve.current--;
	if (table->reserve.current > table->reserve.size) {
		table->reserve.current = table->reserve.size;
	}
}

static void removeFromTableau(KTUI_Table* table, uint8_t index,
		uint8_t first) {
	if (NULL == table || table->tableaus[index].size == 0
			|| first < table->tableaus[index].firstVisible
			|| first >= table->tableaus[index].size) {
		return;
	}
	if (first == table->tableaus[index].firstVisible
			&& table->tableaus[index].firstVisible > 0) {
		table->tableaus[index].firstVisible--;
	}
	table->tableaus[index].size = first;
}

static void placeOnTableau(KTUI_Table* table, uint8_t index,
		uint8_t* cards, uint8_t length) {
	if (NULL == table || (NULL == cards && length > 0)) {
		return;
	}
	memcpy(table->tableaus[index].cards + table->tableaus[index].size,
			cards, length * sizeof(uint8_t));
	if (table->tableaus[index].size == 0) {
		table->tableaus[index].firstVisible = 0;
	}
	table->tableaus[index].size += length;
}

static bool mayBeOnFoundation(KTUI_Table* table, uint8_t index,
		uint8_t card) {
	uint8_t foundSuit, foundNumeral;
	uint8_t cardSuit, cardNumeral;

	if (NULL == table || card >= 52) {
		return false;
	}

	/* Will store garbage if foundation's empty */
	KTUI_GetCardInfo(table->foundations[index], &foundSuit, &foundNumeral);
	KTUI_GetCardInfo(card, &cardSuit, &cardNumeral);
	return (table->foundations[index] >= 52 && cardNumeral == 0)
			|| (foundSuit == cardSuit && foundNumeral + 1 == cardNumeral);
}

static bool mayBeOnTableau(KTUI_Table* table, uint8_t index, uint8_t card) {
	uint8_t tabSuit, tabNumeral;
	uint8_t cardSuit, cardNumeral;

	if (NULL == table || card >= 52) {
		return false;
	}

	KTUI_GetCardInfo(card, &cardSuit, &cardNumeral);
	if (table->tableaus[index].size == 0) {
		return cardNumeral == 12;
	} else {
		KTUI_GetCardInfo(
				table->tableaus[index].cards[table->tableaus[index].size - 1],
				&tabSuit, &tabNumeral);
		return tabNumeral == cardNumeral + 1
				&& ((cardSuit >= 2 && tabSuit <= 1)
						|| (tabSuit >= 2 && cardSuit <= 1));
	}
}
