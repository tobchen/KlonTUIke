/*
 * visual.h
 *
 *  Created on: Jun 20, 2016
 *      Author: tobchen
 */

#ifndef KLONTUIKE_SRC_VISUAL_H
#define KLONTUIKE_SRC_VISUAL_H

#include <time.h>

#include "lv_inter.h"

int KTUI_InitVisual();

void KTUI_QuitVisual();

void KTUI_DrawGame(KTUI_LVInterface* interface);

void KTUI_DrawStart();

void KTUI_DrawWon(time_t playtime);

int KTUI_RequestInput();

#endif /* KLONTUIKE_SRC_VISUAL_H */
