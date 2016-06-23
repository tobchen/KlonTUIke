/*
 * visual.h
 *
 *  Created on: Jun 20, 2016
 *      Author: tobchen
 */

#ifndef KLONTUIKE_SRC_VISUAL_H
#define KLONTUIKE_SRC_VISUAL_H

#include <time.h>

#include "logical.h"

int KlonTUIke_InitVisual();

void KlonTUIke_QuitVisual();

void KlonTUIke_DrawTable(KlonTUIke_Table* table);

void KlonTUIke_DrawStart();

void KlonTUIke_DrawWon(time_t playtime);

int KlonTUIke_RequestInput();

#endif /* KLONTUIKE_SRC_VISUAL_H */
