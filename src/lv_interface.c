/*
 * lv_interface.c
 *
 *  Created on: Jun 24, 2016
 *      Author: tobchen
 */

#include "lv_interface.h"

#include <stdlib.h>

struct klontuike_lv_interface {
	KlonTUIke_Table* table;

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

static void setCursorToTableau(KlonTUIke_LVInterface* interface,
		uint8_t nextTabIndex);

KlonTUIke_LVInterface* KlonTUIke_CreateInterface() {
	KlonTUIke_LVInterface* result = malloc(sizeof(KlonTUIke_LVInterface));
	if (NULL == result) {
		return NULL;
	}
	result->table = KlonTUIke_CreateTable();
	if (NULL == result->table) {
		free(result);
		return NULL;
	}
	return result;
}

void KlonTUIke_DestroyInterface(KlonTUIke_LVInterface* interface) {
	if (interface != NULL) {
		KlonTUIke_DestroyTable(interface->table);
		free(interface);
	}
}

void KlonTUIke_ResetupInterface(KlonTUIke_LVInterface* interface) {
	if (interface != NULL) {
		KlonTUIke_ResetupTable(interface->table);
		interface->cursor = 140;
		interface->selection = 0;
	}
}

void KlonTUIke_CursorUp(KlonTUIke_LVInterface* interface) {
	uint8_t tabIndex;
	uint8_t tabPos;
	KlonTUIke_Table* table;

	if (NULL == interface) {
		return;
	}
	table = interface->table;

	/* Tableau */
	if (interface->cursor < 140) {
		tabIndex = interface->cursor / 20;
		tabPos = interface->cursor % 20;

		if (tabPos == 0
				|| tabPos - 1 <= KlonTUIke_GetTableauFirstVis(table, tabIndex)) {
			interface->cursor = 140 + tabIndex;
		} else {
			interface->cursor = tabIndex * 20 + tabPos - 1;
		}
	/* Reserve or foundation */
	} else {
		tabIndex = interface->cursor - 140;
		interface->cursor =
				tabIndex * 20 + KlonTUIke_GetTableauSize(table, tabIndex);
	}
}

void KlonTUIke_CursorDown(KlonTUIke_LVInterface* interface) {
	uint8_t tabIndex;
	uint8_t tabPos;
	KlonTUIke_Table* table;

	if (NULL == interface) {
		return;
	}
	table = interface->table;

	/* Tableau */
	if (interface->cursor < 140) {
		tabIndex = interface->cursor / 20;
		tabPos = interface->cursor % 20;

		if (tabPos > 0 && tabPos < KlonTUIke_GetTableauSize(table, tabIndex)) {
			interface->cursor = tabIndex * 20 + tabPos + 1;
		} else {
			interface->cursor = 140 + tabIndex;
		}
	/* Reserve or foundation */
	} else {
		tabIndex = interface->cursor - 140;
		if (KlonTUIke_GetTableauSize(table, tabIndex) > 0) {
			interface->cursor = tabIndex * 20
					+ KlonTUIke_GetTableauFirstVis(table, tabIndex) + 1;
		} else {
			interface->cursor = tabIndex * 20;
		}
	}
}

void KlonTUIke_CursorLeft(KlonTUIke_LVInterface* interface) {
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

void KlonTUIke_CursorRight(KlonTUIke_LVInterface* interface) {
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

void KlonTUIke_CursorAction(KlonTUIke_LVInterface* interface) {
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
					if (KlonTUIke_TableauToTableau(interface->table,
							tabIndexFrom, tabPosFrom, tabIndexTo)) {
						if (KlonTUIke_GetTableauSize(interface->table, tabIndexTo) == 1) {
							interface->cursor = tabIndexTo * 20 + 1;
						}
						/* TODO Undo */
					}
				/* Open reserved */
				} else if (interface->selection == 141) {
					if (KlonTUIke_ReserveToTableau(interface->table, tabIndexTo)) {
						if (KlonTUIke_GetTableauSize(interface->table, tabIndexTo) == 1) {
							interface->cursor = tabIndexTo * 20 + 1;
						}
						/* TODO Undo */
					}
				/* Foundation */
				} else if (interface->selection >= 143
						&& interface->selection <= 146) {
					founIndexFrom = interface->selection - 143;
					if (KlonTUIke_FoundationToTableau(interface->table,
							founIndexFrom, tabIndexTo)) {
						if (KlonTUIke_GetTableauSize(interface->table, tabIndexTo) == 1) {
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
					if (KlonTUIke_GetTableauSize(interface->table, tabIndexFrom) - 1
							== tabPosFrom) {
						if (KlonTUIke_TableauToFoundation(interface->table,
								tabIndexFrom, founIndexTo)) {
							/* TODO Undo */
						}
					}
				/* Open reserved */
				} else if (interface->selection == 141) {
					if (KlonTUIke_ReserveToFoundation(interface->table, founIndexTo)) {
						/* TODO Undo */
					}
				/* Foundation */
				} else if (interface->selection >= 143
						&& interface->selection <= 146) {
					founIndexFrom = interface->selection - 143;
					if (KlonTUIke_FoundationToFoundation(interface->table,
							founIndexFrom, founIndexTo)) {
						/* TODO Undo */
					}
				}

			}
		}
		KlonTUIke_CancelSelection(interface);
	} else { /* Card isn't selected */
		/* Tableau */
		if (interface->cursor < 140) {
			if (interface->cursor % 20 > 0) {
				interface->selection = interface->cursor;
			}
		/* Reserved */
		} else if (interface->cursor == 140) {
			KlonTUIke_TurnReserve(interface->table);
		/* Open reserved */
		} else if (interface->cursor == 141 &&
				KlonTUIke_GetOpenReserve(interface->table) < 52) {
			interface->selection = 141;
		/* Foundation */
		} else if (interface->cursor >= 143 && interface->cursor <= 146) {
			if (KlonTUIke_GetFoundation(interface->table, interface->cursor-143) < 52) {
				interface->selection = interface->cursor;
			}
		}
	}
}

void KlonTUIke_CancelSelection(KlonTUIke_LVInterface* interface) {
	if (interface != NULL) {
		interface->selection = 0;
	}
}

bool KlonTUIke_SetCursor(KlonTUIke_LVInterface* interface, uint8_t cursor) {
	uint8_t tabPos, tabIndex;

	if (NULL == interface) {
		return false;
	}

	if (cursor < 140) {
		tabIndex = cursor / 20;
		tabPos = cursor % 20;
		if (tabPos <= KlonTUIke_GetTableauSize(interface->table, tabIndex)
				&& (KlonTUIke_GetTableauSize(interface->table, tabIndex) == 0
						|| tabPos - 1 >= KlonTUIke_GetTableauFirstVis(interface->table, tabIndex))) {
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

uint8_t KlonTUIke_GetCursor(KlonTUIke_LVInterface* interface) {
	if (interface != NULL) {
		return interface->cursor;
	} else {
		return 140;
	}
}

uint8_t KlonTUIke_GetSelection(KlonTUIke_LVInterface* interface) {
	if (interface != NULL) {
		return interface->selection;
	} else {
		return 0;
	}
}

KlonTUIke_Table* KlonTUIke_GetTable(KlonTUIke_LVInterface* interface) {
	if (interface != NULL) {
		return interface->table;
	} else {
		return NULL;
	}
}

static void setCursorToTableau(KlonTUIke_LVInterface* interface,
		uint8_t nextTabIndex) {
	uint8_t nextTabPos;
	KlonTUIke_Table* table = interface->table;

	if (NULL == interface) {
		return;
	}

	if (KlonTUIke_GetTableauSize(table, nextTabIndex) > 0) {
		nextTabPos = interface->cursor % 20;
		if (nextTabPos < KlonTUIke_GetTableauFirstVis(table, nextTabIndex) + 1) {
			nextTabPos = KlonTUIke_GetTableauFirstVis(table, nextTabIndex) + 1;
		} else if (nextTabPos > KlonTUIke_GetTableauSize(table, nextTabIndex)) {
			nextTabPos = KlonTUIke_GetTableauSize(table, nextTabIndex);
		}
	} else {
		nextTabPos = 0;
	}

	interface->cursor = nextTabIndex * 20 + nextTabPos;
}
