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
#include <locale.h>

#ifndef KLONTUIKE_UNICODE
	#include <ncurses.h>
#else
	#include <ncursesw/ncurses.h>
#endif

#include "lv_interface.h"
#include "visual.h"

typedef enum {
	START, INGAME, WON
} KlonTUIke_State;

int main(void) {
	KlonTUIke_State currentState;
	KlonTUIke_LVInterface* interface;
	int input;
	time_t start, end;

	/* ncurses man said to set the locale so why not? */
	setlocale(LC_ALL, "");

	/* Just some randomness */
	srand(time(NULL));

	/* Table */
	interface = KlonTUIke_CreateInterface();
	if (NULL == interface) {
		puts("Failed to create game interface!");
		return EXIT_FAILURE;
	}

	/* Visual */
	if (KlonTUIke_InitVisual()) {
		puts("Failed to setup visuals!");
		KlonTUIke_DestroyInterface(interface);
		return EXIT_FAILURE;
	}

	/* Main loop */
	currentState = START;
	KlonTUIke_DrawStart();
	while (true) {
		input = KlonTUIke_RequestInput();

		/* q = Quit */
		if (input == 113) {
			break;
		/* n = New game */
		} else if (input == 110) {
			KlonTUIke_ResetupInterface(interface);
			currentState = INGAME;
			start = time(NULL);
		} else if (currentState == INGAME) {
			if (input == KEY_LEFT) {
				KlonTUIke_CursorLeft(interface);
			} else if (input == KEY_RIGHT) {
				KlonTUIke_CursorRight(interface);
			} else if (input == KEY_UP) {
				KlonTUIke_CursorUp(interface);
			} else if (input == KEY_DOWN) {
				KlonTUIke_CursorDown(interface);
			/* \n = Enter */
			} else if (input == 10) {
				KlonTUIke_CursorAction(interface);
			} else if (input == KEY_BACKSPACE) {
				KlonTUIke_CancelSelection(interface);
			}
		}

		if (currentState == INGAME
				&& KlonTUIke_HasWon(KlonTUIke_GetTable(interface))) {
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
			KlonTUIke_DrawGame(interface);
		}
	}

	/* Clean up */
	KlonTUIke_QuitVisual();
	KlonTUIke_DestroyInterface(interface);

	return EXIT_SUCCESS;
}
