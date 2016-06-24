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

struct klontuike_table {
	/* Staples */
	Tableau tableaus[7];
	Reserve reserve;

	/* Foundation
	 * Card >= 52 - empty foundation */
	uint8_t foundations[4];

	/* Position of cursor and selection
	 * For every one of the seven tableaus:
	 * 0 - no selection
	 * 0 + (i*20) - ground of tableau
	 * 1-19 + (i*20) - card from first (at the bottom) to last (on top)
	 * 140 - reserved cards
	 * 141 - currently open reserved card
	 * 142 - "nothing" spot between open reserved and first foundation
	 * 143-146 - foundations */
	uint8_t cursor;
	uint8_t selection;
};

static void setCursorToTableau(KlonTUIke_Table* table, uint8_t nextTabIndex);

static void removeFromFoundation(KlonTUIke_Table* table, uint8_t index);
static void removeFromReserve(KlonTUIke_Table* table);
static void removeFromTableau(KlonTUIke_Table* table, uint8_t index,
		uint8_t first);

static void placeOnTableau(KlonTUIke_Table* table, uint8_t index,
		uint8_t* cards, uint8_t length);

static bool mayBeOnFoundation(KlonTUIke_Table* table, uint8_t index,
		uint8_t card);
static bool mayBeOnTableau(KlonTUIke_Table* table, uint8_t index, uint8_t card);

KlonTUIke_Table* KlonTUIke_CreateTable() {
	KlonTUIke_Table* table = malloc(sizeof(KlonTUIke_Table));
	if (NULL == table) {
		return NULL;
	}

	KlonTUIke_ResetupTable(table);

	return table;
}

void KlonTUIke_DestroyTable(KlonTUIke_Table* table) {
	if (table != NULL) {
		free(table);
	}
}

void KlonTUIke_ResetupTable(KlonTUIke_Table* table) {
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

	/* Position/selection */
	table->cursor = 140;
	table->selection = 0;
}

void KlonTUIke_CursorUp(KlonTUIke_Table* table) {
	uint8_t tabIndex;
	uint8_t tabPos;

	if (NULL == table) {
		return;
	}

	/* Tableau */
	if (table->cursor < 140) {
		tabIndex = table->cursor / 20;
		tabPos = table->cursor % 20;

		if (tabPos == 0
				|| tabPos - 1 <= table->tableaus[tabIndex].firstVisible) {
			table->cursor = 140 + tabIndex;
		} else {
			table->cursor = tabIndex * 20 + tabPos - 1;
		}
	/* Reserve or foundation */
	} else {
		tabIndex = table->cursor - 140;
		table->cursor = tabIndex * 20 + table->tableaus[tabIndex].size;
	}
}

void KlonTUIke_CursorDown(KlonTUIke_Table* table) {
	uint8_t tabIndex;
	uint8_t tabPos;

	if (NULL == table) {
		return;
	}

	/* Tableau */
	if (table->cursor < 140) {
		tabIndex = table->cursor / 20;
		tabPos = table->cursor % 20;

		if (tabPos > 0 && tabPos < table->tableaus[tabIndex].size) {
			table->cursor = tabIndex * 20 + tabPos + 1;
		} else {
			table->cursor = 140 + tabIndex;
		}
	/* Reserve or foundation */
	} else {
		tabIndex = table->cursor - 140;
		if (table->tableaus[tabIndex].size > 0) {
			table->cursor = tabIndex * 20
					+ (table->tableaus[tabIndex].firstVisible + 1);
		} else {
			table->cursor = tabIndex * 20;
		}
	}
}

void KlonTUIke_CursorLeft(KlonTUIke_Table* table) {
	uint8_t curTabIndex, nextTabIndex;

	if (NULL == table) {
		return;
	}

	if (table->cursor < 140) { /* Tableau */
		curTabIndex = table->cursor / 20;
		nextTabIndex = curTabIndex - 1;
		if (nextTabIndex >= 7) {
			nextTabIndex = 6;
		}
		setCursorToTableau(table, nextTabIndex);
	} else if (table->cursor >= 140) { /* Reserved or foundation */
		table->cursor--;
		if (table->cursor < 140) {
			table->cursor = 146;
		} else if (table->cursor == 142) {
			table->cursor = 141;
		}
	}
}

void KlonTUIke_CursorRight(KlonTUIke_Table* table) {
	uint8_t curTabIndex, nextTabIndex;

	if (NULL == table) {
		return;
	}

	if (table->cursor < 140) { /* Tableau */
		curTabIndex = table->cursor / 20;
		nextTabIndex = curTabIndex + 1;
		if (nextTabIndex >= 7) {
			nextTabIndex = 0;
		}
		setCursorToTableau(table, nextTabIndex);
	} else if (table->cursor >= 140) { /* Reserved or foundation */
		table->cursor++;
		if (table->cursor > 146) {
			table->cursor = 140;
		} else if (table->cursor == 142) {
			table->cursor = 143;
		}
	}
}

void KlonTUIke_CursorAction(KlonTUIke_Table* table) {
	uint8_t tabIndex, tabIndexFrom;
	uint8_t tabPos, tabPosFrom;
	uint8_t founIndex, founIndexFrom;

	if (NULL == table) {
		return;
	}

	if (table->selection > 0) { /* Card is selected */
		/* Cursor may not be on selection */
		if (table->selection != table->cursor) {
			/* Tableau */
			if (table->cursor < 140) {

				tabIndex = table->cursor / 20;
				/* Tableau */
				if (table->selection < 140) {
					tabIndexFrom = table->selection / 20;
					tabPosFrom = table->selection % 20 - 1;
					if (tabIndexFrom != tabIndex
							&& mayBeOnTableau(table, tabIndex,
									table->tableaus[tabIndexFrom].cards[tabPosFrom])) {
						placeOnTableau(table, tabIndex,
								table->tableaus[tabIndexFrom].cards + tabPosFrom,
								table->tableaus[tabIndexFrom].size - tabPosFrom);
						removeFromTableau(table, tabIndexFrom, tabPosFrom);
					}
				/* Open reserved */
				} else if (table->selection == 141
						&& mayBeOnTableau(table, tabIndex,
								table->reserve.cards[table->reserve.current])) {
					placeOnTableau(table, tabIndex,
							table->reserve.cards + table->reserve.current, 1);
					removeFromReserve(table);
				/* Foundation */
				} else if (table->selection >= 143 && table->selection <= 146) {
					founIndex = table->selection - 143;
					if (mayBeOnTableau(table, tabIndex,
							table->foundations[founIndex])) {
						placeOnTableau(table, tabIndex,
								table->foundations + founIndex, 1);
						removeFromFoundation(table, founIndex);
					}
				}

			/* Foundation */
			} else if (table->cursor >= 143 && table->cursor <= 146) {

				founIndex = table->cursor - 143;
				/* Tableau */
				if (table->selection < 140) {
					tabIndex = table->selection / 20;
					tabPos = table->selection % 20 - 1;
					if (table->tableaus[tabIndex].size - 1 == tabPos
							&& mayBeOnFoundation(table, founIndex,
									table->tableaus[tabIndex].cards[tabPos])) {
						table->foundations[founIndex] =
								table->tableaus[tabIndex].cards[tabPos];
						removeFromTableau(table, tabIndex, tabPos);
					}
				/* Open reserved */
				} else if (table->selection == 141
						&& mayBeOnFoundation(table, founIndex,
								table->reserve.cards[table->reserve.current])) {
					table->foundations[founIndex] =
							table->reserve.cards[table->reserve.current];
					removeFromReserve(table);
				/* Foundation */
				} else if (table->selection >= 143 && table->selection <= 146) {
					founIndexFrom = table->selection - 143;
					if (founIndex != founIndexFrom
							&& mayBeOnFoundation(table, founIndex,
									table->foundations[founIndexFrom])) {
						table->foundations[founIndex] =
								table->foundations[founIndexFrom];
						removeFromFoundation(table, founIndexFrom);
					}
				}

			}
		}
		KlonTUIke_CancelSelection(table);
	} else { /* Card isn't selected */
		if (table->cursor < 140) { /* Tableau */
			if (table->cursor % 20 > 0) {
				table->selection = table->cursor;
			}
		} else if (table->cursor == 140) { /* Reserved */
			table->reserve.current++;
			if (table->reserve.current > table->reserve.size) {
				table->reserve.current = 0;
			}
		} else if (table->cursor == 141) { /* Open reserved */
			if (table->reserve.current < table->reserve.size) {
				table->selection = 141;
			}
		} else if (table->cursor >= 143 && table->cursor <= 146) { /* F */
			if (table->foundations[table->cursor-143] < 52) {
				table->selection = table->cursor;
			}
		}
	}
}

void KlonTUIke_CancelSelection(KlonTUIke_Table* table) {
	if (table != NULL) {
		table->selection = 0;
	}
}

bool KlonTUIke_SetCursor(KlonTUIke_Table* table, uint8_t cursor) {
	uint8_t tabPos;
	Tableau* tableau;

	if (NULL == table) {
		return false;
	}

	if (cursor < 140) {
		tableau = table->tableaus + (cursor / 20);
		tabPos = cursor % 20;
		if (tabPos <= tableau->size
				&& (tableau->size == 0 || tabPos - 1 >= tableau->firstVisible)) {
			table->cursor = cursor;
		}
	} else if (cursor >= 140 && cursor <= 146 && cursor != 142) {
		table->cursor = cursor;
	}

	if (table->cursor == cursor) {
		return true;
	} else {
		return false;
	}
}

uint8_t KlonTUIke_GetFoundation(KlonTUIke_Table* table, uint8_t index) {
	if (NULL == table) {
		return 52;
	} else {
		return table->foundations[index];
	}
}

uint8_t KlonTUIke_GetTableau(KlonTUIke_Table* table, uint8_t index,
		uint8_t position) {
	if (NULL == table || index >= 7 || position >= table->tableaus[index].size) {
		return 52;
	} else if (position < table->tableaus[index].firstVisible) {
		return 53;
	} else {
		return table->tableaus[index].cards[position];
	}
}

uint8_t KlonTUIke_GetOpenReserve(KlonTUIke_Table* table) {
	if (NULL == table || table->reserve.current >= table->reserve.size) {
		return 52;
	} else {
		return table->reserve.cards[table->reserve.current];
	}
}

bool KlonTUIke_IsReserveLeft(KlonTUIke_Table* table) {
	if (NULL == table || table->reserve.size == 0
			|| table->reserve.current + 1 == table->reserve.size) {
		return false;
	} else {
		return true;
	}
}

uint8_t KlonTUIke_GetCursor(KlonTUIke_Table* table) {
	if (NULL == table) {
		return 0;
	}
	return table->cursor;
}

uint8_t KlonTUIke_GetSelection(KlonTUIke_Table* table) {
	if (NULL == table) {
		return 0;
	}
	return table->selection;
}

void KlonTUIke_GetCardInfo(uint8_t card, uint8_t* suit, uint8_t* numeral) {
	if (suit != NULL) {
		*suit = card / 13;
	}
	if (numeral != NULL) {
		*numeral = card % 13;
	}
}

bool KlonTUIke_HasWon(KlonTUIke_Table* table) {
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

static void setCursorToTableau(KlonTUIke_Table* table, uint8_t nextTabIndex) {
	uint8_t nextTabPos;

	if (NULL == table) {
		return;
	}

	if (table->tableaus[nextTabIndex].size > 0) {
		nextTabPos = table->cursor % 20;
		if (nextTabPos < table->tableaus[nextTabIndex].firstVisible + 1) {
			nextTabPos = table->tableaus[nextTabIndex].firstVisible + 1;
		} else if (nextTabPos > table->tableaus[nextTabIndex].size) {
			nextTabPos = table->tableaus[nextTabIndex].size;
		}
	} else {
		nextTabPos = 0;
	}

	table->cursor = nextTabIndex * 20 + nextTabPos;
}

static void removeFromFoundation(KlonTUIke_Table* table, uint8_t index) {
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

static void removeFromReserve(KlonTUIke_Table* table) {
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

static void removeFromTableau(KlonTUIke_Table* table, uint8_t index,
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

static void placeOnTableau(KlonTUIke_Table* table, uint8_t index,
		uint8_t* cards, uint8_t length) {
	if (NULL == table || (NULL == cards && length > 0)) {
		return;
	}
	memcpy(table->tableaus[index].cards + table->tableaus[index].size,
			cards, length * sizeof(uint8_t));
	if (table->tableaus[index].size == 0) {
		table->tableaus[index].firstVisible = 0;
		table->cursor = index * 20 + 1;
	}
	table->tableaus[index].size += length;
}

static bool mayBeOnFoundation(KlonTUIke_Table* table, uint8_t index,
		uint8_t card) {
	uint8_t foundSuit, foundNumeral;
	uint8_t cardSuit, cardNumeral;

	if (NULL == table) {
		return false;
	}

	/* Will store garbage if foundation's empty */
	KlonTUIke_GetCardInfo(table->foundations[index], &foundSuit, &foundNumeral);
	KlonTUIke_GetCardInfo(card, &cardSuit, &cardNumeral);
	return (table->foundations[index] >= 52 && cardNumeral == 0)
			|| (foundSuit == cardSuit && foundNumeral + 1 == cardNumeral);
}

static bool mayBeOnTableau(KlonTUIke_Table* table, uint8_t index, uint8_t card) {
	uint8_t tabSuit, tabNumeral;
	uint8_t cardSuit, cardNumeral;

	if (NULL == table) {
		return false;
	}

	KlonTUIke_GetCardInfo(card, &cardSuit, &cardNumeral);
	if (table->tableaus[index].size == 0) {
		return cardNumeral == 12;
	} else {
		KlonTUIke_GetCardInfo(
				table->tableaus[index].cards[table->tableaus[index].size - 1],
				&tabSuit, &tabNumeral);
		return tabNumeral == cardNumeral + 1
				&& ((cardSuit >= 2 && tabSuit <= 1)
						|| (tabSuit >= 2 && cardSuit <= 1));
	}
}
