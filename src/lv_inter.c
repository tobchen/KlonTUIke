/*
 * lv_interface.c
 *
 *  Created on: Jun 24, 2016
 *      Author: tobchen
 */

#include "lv_inter.h"

#include <stdlib.h>

struct ktui_lv_interface {
	KTUI_Table* table;

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

static void setCursorToTableau(KTUI_LVInterface* interface,
		uint8_t nextTabIndex);

KTUI_LVInterface* KTUI_CreateInterface() {
	KTUI_LVInterface* result = malloc(sizeof(KTUI_LVInterface));
	if (NULL == result) {
		return NULL;
	}
	result->table = KTUI_CreateTable();
	if (NULL == result->table) {
		free(result);
		return NULL;
	}
	return result;
}

void KTUI_DestroyInterface(KTUI_LVInterface* interface) {
	if (interface != NULL) {
		KTUI_DestroyTable(interface->table);
		free(interface);
	}
}

void KTUI_ResetupInterface(KTUI_LVInterface* interface) {
	if (interface != NULL) {
		KTUI_ResetupTable(interface->table);
		interface->cursor = 140;
		interface->selection = 0;
	}
}

void KTUI_CursorUp(KTUI_LVInterface* interface) {
	uint8_t tabIndex;
	uint8_t tabPos;
	KTUI_Table* table;

	if (NULL == interface) {
		return;
	}
	table = interface->table;

	/* Tableau */
	if (interface->cursor < 140) {
		tabIndex = interface->cursor / 20;
		tabPos = interface->cursor % 20;

		if (tabPos == 0
				|| tabPos - 1 <= KTUI_GetTableauFirstVis(table, tabIndex)) {
			interface->cursor = 140 + tabIndex;
		} else {
			interface->cursor = tabIndex * 20 + tabPos - 1;
		}
	/* Reserve or foundation */
	} else {
		tabIndex = interface->cursor - 140;
		interface->cursor =
				tabIndex * 20 + KTUI_GetTableauSize(table, tabIndex);
	}
}

void KTUI_CursorDown(KTUI_LVInterface* interface) {
	uint8_t tabIndex;
	uint8_t tabPos;
	KTUI_Table* table;

	if (NULL == interface) {
		return;
	}
	table = interface->table;

	/* Tableau */
	if (interface->cursor < 140) {
		tabIndex = interface->cursor / 20;
		tabPos = interface->cursor % 20;

		if (tabPos > 0 && tabPos < KTUI_GetTableauSize(table, tabIndex)) {
			interface->cursor = tabIndex * 20 + tabPos + 1;
		} else {
			interface->cursor = 140 + tabIndex;
		}
	/* Reserve or foundation */
	} else {
		tabIndex = interface->cursor - 140;
		if (KTUI_GetTableauSize(table, tabIndex) > 0) {
			interface->cursor = tabIndex * 20
					+ KTUI_GetTableauFirstVis(table, tabIndex) + 1;
		} else {
			interface->cursor = tabIndex * 20;
		}
	}
}

void KTUI_CursorLeft(KTUI_LVInterface* interface) {
	uint8_t curTabIndex, nextTabIndex;

	if (NULL == interface) {
		return;
	}

	if (interface->cursor < 140) { /* Tableau */
		curTabIndex = interface->cursor / 20;
		nextTabIndex = curTabIndex - 1;
		if (nextTabIndex >= 7) {
			nextTabIndex = 6;
		}
		setCursorToTableau(interface, nextTabIndex);
	} else if (interface->cursor >= 140) { /* Reserved or foundation */
		interface->cursor--;
		if (interface->cursor < 140) {
			interface->cursor = 146;
		} else if (interface->cursor == 142) {
			interface->cursor = 141;
		}
	}
}

void KTUI_CursorRight(KTUI_LVInterface* interface) {
	uint8_t curTabIndex, nextTabIndex;

	if (NULL == interface) {
		return;
	}

	if (interface->cursor < 140) { /* Tableau */
		curTabIndex = interface->cursor / 20;
		nextTabIndex = curTabIndex + 1;
		if (nextTabIndex >= 7) {
			nextTabIndex = 0;
		}
		setCursorToTableau(interface, nextTabIndex);
	} else if (interface->cursor >= 140) { /* Reserved or foundation */
		interface->cursor++;
		if (interface->cursor > 146) {
			interface->cursor = 140;
		} else if (interface->cursor == 142) {
			interface->cursor = 143;
		}
	}
}

void KTUI_CursorAction(KTUI_LVInterface* interface) {
	uint8_t tabIndexTo;
	uint8_t tabIndexFrom, tabPosFrom;
	uint8_t founIndexTo, founIndexFrom;

	if (NULL == interface) {
		return;
	}

	if (interface->selection > 0) { /* Card is selected */
		/* Cursor may not be on selection */
		if (interface->selection != interface->cursor) {
			/* Tableau */
			if (interface->cursor < 140) {
				tabIndexTo = interface->cursor / 20;
				/* Tableau */
				if (interface->selection < 140) {
					tabIndexFrom = interface->selection / 20;
					tabPosFrom = interface->selection % 20 - 1;
					if (KTUI_TableauToTableau(interface->table,
							tabIndexFrom, tabPosFrom, tabIndexTo)) {
						if (KTUI_GetTableauSize(interface->table, tabIndexTo) == 1) {
							interface->cursor = tabIndexTo * 20 + 1;
						}
						/* TODO Undo */
					}
				/* Open reserved */
				} else if (interface->selection == 141) {
					if (KTUI_ReserveToTableau(interface->table, tabIndexTo)) {
						if (KTUI_GetTableauSize(interface->table, tabIndexTo) == 1) {
							interface->cursor = tabIndexTo * 20 + 1;
						}
						/* TODO Undo */
					}
				/* Foundation */
				} else if (interface->selection >= 143
						&& interface->selection <= 146) {
					founIndexFrom = interface->selection - 143;
					if (KTUI_FoundationToTableau(interface->table,
							founIndexFrom, tabIndexTo)) {
						if (KTUI_GetTableauSize(interface->table, tabIndexTo) == 1) {
							interface->cursor = tabIndexTo * 20 + 1;
						}
						/* TODO Undo */
					}
				}
			/* Foundation */
			} else if (interface->cursor >= 143 && interface->cursor <= 146) {
				founIndexTo = interface->cursor - 143;
				/* Tableau */
				if (interface->selection < 140) {
					tabIndexFrom = interface->selection / 20;
					tabPosFrom = interface->selection % 20 - 1;
					if (KTUI_GetTableauSize(interface->table, tabIndexFrom) - 1
							== tabPosFrom) {
						if (KTUI_TableauToFoundation(interface->table,
								tabIndexFrom, founIndexTo)) {
							/* TODO Undo */
						}
					}
				/* Open reserved */
				} else if (interface->selection == 141) {
					if (KTUI_ReserveToFoundation(interface->table, founIndexTo)) {
						/* TODO Undo */
					}
				/* Foundation */
				} else if (interface->selection >= 143
						&& interface->selection <= 146) {
					founIndexFrom = interface->selection - 143;
					if (KTUI_FoundationToFoundation(interface->table,
							founIndexFrom, founIndexTo)) {
						/* TODO Undo */
					}
				}

			}
		}
		KTUI_CancelSelection(interface);
	} else { /* Card isn't selected */
		/* Tableau */
		if (interface->cursor < 140) {
			if (interface->cursor % 20 > 0) {
				interface->selection = interface->cursor;
			}
		/* Reserved */
		} else if (interface->cursor == 140) {
			KTUI_TurnReserve(interface->table);
		/* Open reserved */
		} else if (interface->cursor == 141 &&
				KTUI_GetOpenReserve(interface->table) < 52) {
			interface->selection = 141;
		/* Foundation */
		} else if (interface->cursor >= 143 && interface->cursor <= 146) {
			if (KTUI_GetFoundation(interface->table, interface->cursor-143) < 52) {
				interface->selection = interface->cursor;
			}
		}
	}
}

void KTUI_CancelSelection(KTUI_LVInterface* interface) {
	if (interface != NULL) {
		interface->selection = 0;
	}
}

bool KTUI_SetCursor(KTUI_LVInterface* interface, uint8_t cursor) {
	uint8_t tabPos, tabIndex;

	if (NULL == interface) {
		return false;
	}

	if (cursor < 140) {
		tabIndex = cursor / 20;
		tabPos = cursor % 20;
		if ((tabPos == 0 && KTUI_GetTableauSize(interface->table, tabIndex))
				|| (tabPos-1 >= KTUI_GetTableauFirstVis(interface->table, tabIndex)
				&& tabPos <= KTUI_GetTableauSize(interface->table, tabIndex))) {
			interface->cursor = cursor;
		}
	} else if (cursor >= 140 && cursor <= 146 && cursor != 142) {
		interface->cursor = cursor;
	}

	if (interface->cursor == cursor) {
		return true;
	} else {
		return false;
	}
}

uint8_t KTUI_GetCursor(KTUI_LVInterface* interface) {
	if (interface != NULL) {
		return interface->cursor;
	} else {
		return 140;
	}
}

uint8_t KTUI_GetSelection(KTUI_LVInterface* interface) {
	if (interface != NULL) {
		return interface->selection;
	} else {
		return 0;
	}
}

KTUI_Table* KTUI_GetTable(KTUI_LVInterface* interface) {
	if (interface != NULL) {
		return interface->table;
	} else {
		return NULL;
	}
}

static void setCursorToTableau(KTUI_LVInterface* interface,
		uint8_t nextTabIndex) {
	uint8_t nextTabPos;
	KTUI_Table* table = interface->table;

	if (NULL == interface) {
		return;
	}

	if (KTUI_GetTableauSize(table, nextTabIndex) > 0) {
		nextTabPos = interface->cursor % 20;
		if (nextTabPos < KTUI_GetTableauFirstVis(table, nextTabIndex) + 1) {
			nextTabPos = KTUI_GetTableauFirstVis(table, nextTabIndex) + 1;
		} else if (nextTabPos > KTUI_GetTableauSize(table, nextTabIndex)) {
			nextTabPos = KTUI_GetTableauSize(table, nextTabIndex);
		}
	} else {
		nextTabPos = 0;
	}

	interface->cursor = nextTabIndex * 20 + nextTabPos;
}
