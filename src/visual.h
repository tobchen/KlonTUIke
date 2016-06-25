/*
 * visual.h
 *
 *  Created on: Jun 20, 2016
 *      Author: tobchen
 */

#ifndef KLONTUIKE_SRC_VISUAL_H
#define KLONTUIKE_SRC_VISUAL_H

#include <time.h>

#include "lv_interface.h"

int KlonTUIke_InitVisual();

void KlonTUIke_QuitVisual();

void KlonTUIke_DrawGame(KlonTUIke_LVInterface* interface);

void KlonTUIke_DrawStart();

void KlonTUIke_DrawWon(time_t playtime);

int KlonTUIke_RequestInput();

#endif /* KLONTUIKE_SRC_VISUAL_H */
