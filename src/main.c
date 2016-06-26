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

#ifndef KTUI_UNICODE
	#include <ncurses.h>
#else
	#include <ncursesw/ncurses.h>
#endif

#include "lv_inter.h"
#include "visual.h"

typedef enum {
	START, INGAME, WON
} KlonTUIke_State;

int main(void) {
	KlonTUIke_State currentState;
	KTUI_LVInterface* interface;
	int input;
	time_t start, end;

	/* ncurses man said to set the locale so why not? */
	setlocale(LC_ALL, "");

	/* Just some randomness */
	srand(time(NULL));

	/* Table */
	interface = KTUI_CreateInterface();
	if (NULL == interface) {
		puts("Failed to create game interface!");
		return EXIT_FAILURE;
	}

	/* Visual */
	if (KTUI_InitVisual()) {
		puts("Failed to setup visuals!");
		KTUI_DestroyInterface(interface);
		return EXIT_FAILURE;
	}

	/* Main loop */
	currentState = START;
	KTUI_DrawStart();
	while (true) {
		input = KTUI_RequestInput();

		/* q = Quit */
		if (input == 113) {
			break;
		/* n = New game */
		} else if (input == 110) {
			KTUI_ResetupInterface(interface);
			currentState = INGAME;
			start = time(NULL);
		} else if (currentState == INGAME) {
			if (input == KEY_LEFT) {
				KTUI_CursorLeft(interface);
			} else if (input == KEY_RIGHT) {
				KTUI_CursorRight(interface);
			} else if (input == KEY_UP) {
				KTUI_CursorUp(interface);
			} else if (input == KEY_DOWN) {
				KTUI_CursorDown(interface);
			/* \n = Enter */
			} else if (input == 10) {
				KTUI_CursorAction(interface);
			} else if (input == KEY_BACKSPACE) {
				KTUI_CancelSelection(interface);
			}
		}

		if (currentState == INGAME
				&& KTUI_HasWon(KTUI_GetTable(interface))) {
			end = time(NULL);
			currentState = WON;
		}

		switch (currentState) {
		case START:
			KTUI_DrawStart();
			break;
		case WON:
			KTUI_DrawWon(end - start);
			break;
		default:
			KTUI_DrawGame(interface);
		}
	}

	/* Clean up */
	KTUI_QuitVisual();
	KTUI_DestroyInterface(interface);

	return EXIT_SUCCESS;
}
