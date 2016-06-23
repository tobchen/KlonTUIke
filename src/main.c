/*
 * main.c
 *
 *  Created on: Jun 20, 2016
 *      Author: tobchen
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#ifndef KLONTUIKE_UNICODE
	#include <ncurses.h>
#else
	#include <ncursesw/ncurses.h>
	#include <locale.h>
#endif

#include "logical.h"
#include "visual.h"

typedef enum {
	START, INGAME, WON
} KlonTUIke_State;

int main(void) {
	KlonTUIke_State currentState;
	KlonTUIke_Table* table;
	int input;
	time_t start, end;

	#ifdef KLONTUIKE_UNICODE
		setlocale(LC_ALL, "");
	#endif

	/* Just some randomness */
	srand(time(NULL));

	/* Table */
	table = KlonTUIke_CreateTable();
	if (NULL == table) {
		puts("Failed to create game table!");
		return EXIT_FAILURE;
	}

	/* Visual */
	if (KlonTUIke_InitVisual()) {
		puts("Failed to setup visuals!");
		KlonTUIke_DestroyTable(table);
		return EXIT_FAILURE;
	}

	/* Main loop */
	currentState = START;
	KlonTUIke_DrawStart();
	while (true) {
		input = KlonTUIke_RequestInput();

		if (input == 113) { /* q = Quit */
			break;
		} else if (input == 110) { /* n = New game */
			KlonTUIke_ResetupTable(table);
			currentState = INGAME;
			start = time(NULL);
		} else if (currentState == INGAME) {
			if (input == KEY_LEFT) {
				KlonTUIke_CursorLeft(table);
			} else if (input == KEY_RIGHT) {
				KlonTUIke_CursorRight(table);
			} else if (input == KEY_UP) {
				KlonTUIke_CursorUp(table);
			} else if (input == KEY_DOWN) {
				KlonTUIke_CursorDown(table);
			} else if (input == 10) { /* \n = Enter */
				KlonTUIke_CursorAction(table);
			} else if (input == KEY_BACKSPACE) {
				KlonTUIke_CancelSelection(table);
			}
		}

		if (currentState == INGAME && KlonTUIke_HasWon(table)) {
			end = time(NULL);
			currentState = WON;
		}

		switch (currentState) {
		case START:
			KlonTUIke_DrawStart();
			break;
		case WON:
			KlonTUIke_DrawWon(end - start);
			break;
		default:
			KlonTUIke_DrawTable(table);
		}
	}

	/* Clean up */
	KlonTUIke_QuitVisual();
	KlonTUIke_DestroyTable(table);

	return EXIT_SUCCESS;
}
