/*
 * visual.c
 *
 *  Created on: Jun 20, 2016
 *      Author: tobchen
 */

#define KLONTUIKE_COLOR_WHITE 5
#define KLONTUIKE_COLOR_EMPTY 4
#define KLONTUIKE_COLOR_BACK 3
#define KLONTUIKE_COLOR_BLACK 2
#define KLONTUIKE_COLOR_RED 1

#include "visual.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef KLONTUIKE_UNICODE
	#include <ncurses.h>
#else
	#include <ncursesw/ncurses.h>
#endif

static WINDOW* window;

static void drawCard(int y, int x, uint8_t card, bool selected);
static void drawBack(int y, int x);
static void drawEmpty(int y, int x);

int KlonTUIke_InitVisual() {
	/* Initialize curses */
	if (initscr() == NULL) {
		return -1;
	}
	if (cbreak() == ERR || noecho() == ERR) {
		endwin();
		return -1;
	}

	/* Ensure minimum size for terminal */
	if (COLS < 27 || LINES < 21) {
		endwin();
		return -1;
	}

	/* Initialize colors */
	start_color();
	if (!has_colors()) {
		endwin();
		return -1;
	}
	init_pair(KLONTUIKE_COLOR_RED, COLOR_RED, COLOR_WHITE);
	init_pair(KLONTUIKE_COLOR_BLACK, COLOR_BLACK, COLOR_WHITE);
	init_pair(KLONTUIKE_COLOR_BACK, COLOR_WHITE, COLOR_BLUE);
	init_pair(KLONTUIKE_COLOR_EMPTY, COLOR_BLUE, COLOR_BLACK);
	init_pair(KLONTUIKE_COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);

	/* Create window */
	window = newwin(21, 27, (LINES - 21) / 2, (COLS - 27) / 2);
	if (NULL == window) {
		endwin();
		return -1;
	}
	keypad(window, TRUE);

	return 0;
}

void KlonTUIke_QuitVisual() {
	delwin(window);
	endwin();
}

void KlonTUIke_DrawTable(KlonTUIke_Table* table) {
	uint8_t i, j;
	uint8_t card;
	uint8_t position;
	#ifdef KLONTUIKE_DEBUG
		char cursorPos[8];
	#endif

	wclear(window);

	/* Reserve */
	if (KlonTUIke_IsReserveLeft(table)) {
		drawBack(0, 0);
	} else {
		drawEmpty(0, 0);
	}
	card = KlonTUIke_GetOpenReserve(table);
	if (card < 52) {
		drawCard(0, 4, card, false);
	} else {
		drawEmpty(0, 4);
	}

	/* Foundation */
	for (i = 0; i < 4; i++) {
		j = KlonTUIke_GetFoundation(table, i);
		if (j < 52) { /* Card on foundation */
			drawCard(0, 12 + i * 4, j, false);
		} else { /* Empty */
			drawEmpty(0, 12 + i * 4);
		}
	}

	/* Tableaus */
	for (i = 0; i < 7; i++) {
		for (j = 0;; j++) {
			card = KlonTUIke_GetTableau(table, i, j);
			if (card == 52) {
				break;
			} else if (card == 53) {
				drawBack(2 + j, i * 4);
			} else {
				drawCard(2 + j, i * 4, card, false);
			}
		}
		if (j == 0) {
			drawEmpty(2, i * 4);
		}
	}

	#ifdef KLONTUIKE_DEBUG
		sprintf(cursorPos, "%u,%u", KlonTUIke_GetCursor(table),
				KlonTUIke_GetSelection(table));
		wattrset(window, COLOR_PAIR(KLONTUIKE_COLOR_WHITE));
		mvwaddstr(window, 20, 0, cursorPos);
	#endif

	/* Selection */
	position = KlonTUIke_GetSelection(table);
	if (position > 0) {
		/* Tableau */
		if (position < 140) {
			i = position / 20;
			j = position % 20 - 1;
			drawCard(2 + j, i * 4, KlonTUIke_GetTableau(table, i, j), true);
		/* Open Reserved */
		} else if (position == 141) {
			drawCard(0, 4, KlonTUIke_GetOpenReserve(table), true);
		} else if (position <= 145) { /* Foundation */
			i = position - 142;
			drawCard(0, 12 + i * 4, KlonTUIke_GetFoundation(table, i), true);
		}
	}

	/* Cursor */
	position = KlonTUIke_GetCursor(table);
	if (position < 140) { /* Tableau */
		i = position / 20;
		j = position % 20;
		if (j == 0) { /* Empty */
			wmove(window, 2, i * 4);
		} else { /* Non-empty */
			wmove(window, 2 + j - 1, i * 4);
		}
	} else if (position == 140) { /* Reserved */
		wmove(window, 0, 0);
	} else if (position == 141) { /* Open Reserved */
		wmove(window, 0, 4);
	} else if (position <= 145) { /* Foundation */
		wmove(window, 0, 12 + (position - 142) * 4);
	}

	curs_set(2);
	wrefresh(window);
}

void KlonTUIke_DrawStart() {
	wclear(window);

	wattrset(window, COLOR_PAIR(KLONTUIKE_COLOR_WHITE));
	mvwaddstr(window, 6, 4, "KlonTUIke (Solitaire)");
	wattrset(window, COLOR_PAIR(KLONTUIKE_COLOR_EMPTY));
	mvwaddstr(window, 7, 12, "by @TobchenDe");

	wattrset(window, COLOR_PAIR(KLONTUIKE_COLOR_WHITE));
	mvwaddstr(window, 9, 0, "n - New game");
	mvwaddstr(window, 10, 0, "q - Quit");

	mvwaddstr(window, 12, 0, "Enter - Action");
	mvwaddstr(window, 13, 0, "BKSP - Cancel selection");

	curs_set(0);
	wrefresh(window);
}

void KlonTUIke_DrawWon(time_t playtime) {
	char playtimeText[20];
	int playtimeX;

	if (playtime < 500000) {
		playtimeX = sprintf(playtimeText, "Playtime: %ldm%lds",
				playtime / 60, playtime % 60);
	} else {
		playtimeX = sprintf(playtimeText, "You took a long time!");
	}
	playtimeX = (27 - playtimeX) / 2;

	wclear(window);

	wattrset(window, COLOR_PAIR(KLONTUIKE_COLOR_WHITE));
	mvwaddstr(window, 9, 9, "You won!");
	wattrset(window, COLOR_PAIR(KLONTUIKE_COLOR_EMPTY));
	mvwaddstr(window, 10, playtimeX, playtimeText);

	curs_set(0);
	wrefresh(window);
}

int KlonTUIke_RequestInput() {
	return wgetch(window);
}

#ifndef KLONTUIKE_UNICODE
static void drawCard(int y, int x, uint8_t card, bool selected) {
	int underlined = selected ? A_UNDERLINE : 0;
	char text[3] = { ' ', ' ', ' ' };

	/* Set color */
	if (card < 26) {
		wattrset(window, COLOR_PAIR(KLONTUIKE_COLOR_RED) | underlined);
	} else {
		wattrset(window, COLOR_PAIR(KLONTUIKE_COLOR_BLACK) | underlined);
	}

	/* Set suit */
	switch (card / 13) {
	case 0: /* Hearts */
		text[0] = 'H';
		break;
	case 1: /* Diamonds */
		text[0] = 'D';
		break;
	case 2: /* Clubs */
		text[0] = 'C';
		break;
	default: /* Spades */
		text[0] = 'S';
		break;
	}

	/* Set numeral */
	switch (card % 13) {
	case 0:
		text[2] = 'A';
		break;
	case 9:
		text[1] = '1';
		text[2] = '0';
		break;
	case 10:
		text[2] = 'J';
		break;
	case 11:
		text[2] = 'Q';
		break;
	case 12:
		text[2] = 'K';
		break;
	default:
		text[2] = (card % 13) + 49;
	}

	/* And print */
	mvwaddnstr(window, y, x, text, 3);
}
#else
static void drawCard(int y, int x, uint8_t card, bool selected) {
	int underlined = selected ? A_UNDERLINE : 0;
	char text[5] = { (unsigned char) 0xE2, (unsigned char) 0x99, 0, ' ', ' ' };

	/* Set color */
	if (card < 26) {
		wattrset(window, COLOR_PAIR(KLONTUIKE_COLOR_RED) | underlined);
	} else {
		wattrset(window, COLOR_PAIR(KLONTUIKE_COLOR_BLACK) | underlined);
	}

	/* Set suit */
	switch (card / 13) {
	case 0: /* Hearts */
		text[2] = (unsigned char) 0xA5;
		break;
	case 1: /* Diamonds */
		text[2] = (unsigned char) 0xA6;
		break;
	case 2: /* Clubs */
		text[2] = (unsigned char) 0xA3;
		break;
	default: /* Spades */
		text[2] = (unsigned char) 0xA0;
		break;
	}

	/* Set numeral */
	switch (card % 13) {
	case 0:
		text[4] = 'A';
		break;
	case 9:
		text[3] = '1';
		text[4] = '0';
		break;
	case 10:
		text[4] = 'J';
		break;
	case 11:
		text[4] = 'Q';
		break;
	case 12:
		text[4] = 'K';
		break;
	default:
		text[4] = (card % 13) + 49;
	}

	/* And print */
	mvwaddnstr(window, y, x, text, 5);
}
#endif

static void drawBack(int y, int x) {
	wattrset(window, COLOR_PAIR(KLONTUIKE_COLOR_BACK));
	mvwaddnstr(window, y, x, "   ", 3);
}

static void drawEmpty(int y, int x) {
	wattrset(window, COLOR_PAIR(KLONTUIKE_COLOR_EMPTY));
	mvwaddnstr(window, y, x, "[ ]", 3);
}
