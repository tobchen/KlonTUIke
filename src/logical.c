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

struct klontuike_stack {
	uint8_t cards[24];
	uint8_t size;
	uint8_t firstVisible;
};

struct klontuike_table {
	/* Staples */
	KlonTUIke_Stack tableaus[7];
	KlonTUIke_Stack reserve;

	/* Foundation
	 * Card >= 52 - empty foundation */
	uint8_t foundations[4];

	/* Position of cursor and selection
	 * For every one of the seven tableaus:
	 * 0 - no selection
	 * 0 + (i*20) - ground of plateau
	 * 1-19 + (i*20) - card from first (at the bottom) to last (on top)
	 * 140 - reserved cards
	 * 141 - currently open reserved card
	 * 142-145 - foundations */
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
	table->reserve.firstVisible = 24;

	/* Position/selection */
	table->cursor = 140;
	table->selection = 0;
}

void KlonTUIke_CursorUp(KlonTUIke_Table* table) {
	uint8_t tabIndex;
	uint8_t tabPos;

	if (table->cursor < 140) { /* Tableau */
		tabIndex = table->cursor / 20;
		tabPos = table->cursor % 20;

		if (tabPos == 0
				|| tabPos - 1 <= table->tableaus[tabIndex].firstVisible) {
			if (tabIndex < 2) {
				table->cursor = 140 + tabIndex;
			} else if (tabIndex > 2) {
				table->cursor = 139 + tabIndex;
			} else {
				table->cursor = 141;
			}
		} else {
			table->cursor = tabIndex * 20 + tabPos - 1;
		}
	} else if (table->cursor <= 145) { /* Reserve or foundation */
		if (table->cursor <= 141) { /* (Open) reserve */
			tabIndex = table->cursor - 140;
		} else { /* Foundation */
			tabIndex = table->cursor - 142 + 3;
		}
		table->cursor = tabIndex * 20 + table->tableaus[tabIndex].size;
	}
}

void KlonTUIke_CursorDown(KlonTUIke_Table* table) {
	uint8_t tabIndex;
	uint8_t tabPos;

	if (table->cursor < 140) { /* Tableau */
		tabIndex = table->cursor / 20;
		tabPos = table->cursor % 20;

		if (tabPos > 0 && tabPos < table->tableaus[tabIndex].size) {
			table->cursor = tabIndex * 20 + tabPos + 1;
		} else if (tabIndex < 2) {
			table->cursor = 140 + tabIndex;
		} else if (tabIndex > 2) {
			table->cursor = 139 + tabIndex;
		} else {
			table->cursor = 141;
		}
	} else if (table->cursor <= 145) { /* Reserve or foundation */
		if (table->cursor <= 141) { /* (Open) reserve */
			tabIndex = table->cursor - 140;
		} else { /* Foundation */
			tabIndex = table->cursor - 142 + 3;
		}
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
			table->cursor = 145;
		}
	}
}

void KlonTUIke_CursorRight(KlonTUIke_Table* table) {
	uint8_t curTabIndex, nextTabIndex;

	if (table->cursor < 140) { /* Tableau */
		curTabIndex = table->cursor / 20;
		nextTabIndex = curTabIndex + 1;
		if (nextTabIndex >= 7) {
			nextTabIndex = 0;
		}
		setCursorToTableau(table, nextTabIndex);
	} else if (table->cursor >= 140) { /* Reserved or foundation */
		table->cursor++;
		if (table->cursor > 145) {
			table->cursor = 140;
		}
	}
}

void KlonTUIke_CursorAction(KlonTUIke_Table* table) {
	uint8_t tabIndex, tabIndexFrom;
	uint8_t tabPos, tabPosFrom;
	uint8_t founIndex, founIndexFrom;

	if (table->selection > 0) { /* Card is selected */
		/* Cursor may not be on selection */
		if (table->selection != table->cursor) {
			if (table->cursor < 140) { /* Tableau */

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
								table->reserve.cards[table->reserve.firstVisible])) {
					placeOnTableau(table, tabIndex,
							table->reserve.cards + table->reserve.firstVisible, 1);
					removeFromReserve(table);
				/* Foundation */
				} else if (table->selection >= 142 && table->selection <= 145) {
					founIndex = table->selection - 142;
					if (mayBeOnTableau(table, tabIndex,
							table->foundations[founIndex])) {
						placeOnTableau(table, tabIndex,
								table->foundations + founIndex, 1);
						removeFromFoundation(table, founIndex);
					}
				}

			} else if (table->cursor >= 142 && table->cursor <= 145) { /* F */

				founIndex = table->cursor - 142;
				if (table->selection < 140) { /* Tableau */
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
								table->reserve.cards[table->reserve.firstVisible])) {
					table->foundations[founIndex] =
							table->reserve.cards[table->reserve.firstVisible];
					removeFromReserve(table);
				/* Foundation */
				} else if (table->selection >= 142 && table->selection <= 145) {
					founIndexFrom = table->selection - 142;
					if (mayBeOnFoundation(table, founIndex,
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
			table->reserve.firstVisible++;
			if (table->reserve.firstVisible > table->reserve.size) {
				table->reserve.firstVisible = 0;
			}
		} else if (table->cursor == 141) { /* Open reserved */
			if (table->reserve.firstVisible < table->reserve.size) {
				table->selection = 141;
			}
		} else if (table->cursor >= 142 && table->cursor <= 145) { /* F */
			if (table->foundations[table->cursor-142] < 52) {
				table->selection = table->cursor;
			}
		}
	}
}

void KlonTUIke_CancelSelection(KlonTUIke_Table* table) {
	table->selection = 0;
}

KlonTUIke_Stack* KlonTUIke_GetTableau(KlonTUIke_Table* table, uint8_t index) {
	return &(table->tableaus[index]);
}

KlonTUIke_Stack* KlonTUIke_GetReserve(KlonTUIke_Table* table) {
	return &(table->reserve);
}

uint8_t KlonTUIke_GetFoundation(KlonTUIke_Table* table, uint8_t index) {
	return table->foundations[index];
}

uint8_t KlonTUIke_GetCard(KlonTUIke_Stack* stack, uint8_t index) {
	return stack->cards[index];
}

uint8_t KlonTUIke_GetSize(KlonTUIke_Stack* stack) {
	return stack->size;
}

uint8_t KlonTUIke_GetFirstVisible(KlonTUIke_Stack* stack) {
	return stack->firstVisible;
}

uint8_t KlonTUIke_GetCursor(KlonTUIke_Table* table) {
	return table->cursor;
}

uint8_t KlonTUIke_GetSelection(KlonTUIke_Table* table) {
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
	for (i = 0; i < 4; i++) {
		if (table->foundations[i] >= 52 || table->foundations[i] % 13 != 12) {
			return false;
		}
	}
	return true;
}

static void setCursorToTableau(KlonTUIke_Table* table, uint8_t nextTabIndex) {
	uint8_t nextTabPos;

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
	if (table->foundations[index] >= 52) {
		return;
	}
	if (table->foundations[index] % 13 == 0) { /* Remove ace */
		table->foundations[index] = 52;
	} else { /* Remove 2 or bigger */
		table->foundations[index]--;
	}
}

static void removeFromReserve(KlonTUIke_Table* table) {
	if (table->reserve.firstVisible >= table->reserve.size) {
		return;
	}
	memmove(table->reserve.cards + table->reserve.firstVisible,
			table->reserve.cards + table->reserve.firstVisible + 1,
			table->reserve.size - table->reserve.firstVisible - 1);
	table->reserve.size--;
	table->reserve.firstVisible--;
	if (table->reserve.firstVisible > table->reserve.size) {
		table->reserve.firstVisible = table->reserve.size;
	}
}

static void removeFromTableau(KlonTUIke_Table* table, uint8_t index,
		uint8_t first) {
	if (table->tableaus[index].size == 0
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

	/* Will store garbage if foundation's empty */
	KlonTUIke_GetCardInfo(table->foundations[index], &foundSuit, &foundNumeral);
	KlonTUIke_GetCardInfo(card, &cardSuit, &cardNumeral);
	return (table->foundations[index] >= 52 && cardNumeral == 0)
			|| (foundSuit == cardSuit && foundNumeral + 1 == cardNumeral);
}

static bool mayBeOnTableau(KlonTUIke_Table* table, uint8_t index, uint8_t card) {
	uint8_t tabSuit, tabNumeral;
	uint8_t cardSuit, cardNumeral;

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