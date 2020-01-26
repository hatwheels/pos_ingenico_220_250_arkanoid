/*
 * utility.h
 *
 *  Created on: Nov 15, 2019
 *      Author: hatwheels
 *      Based on TRAINING module of Ingenico
 */

#include "basearm.h"

#define NUMBER_OF_ITEMS(a) (sizeof(a)/sizeof((a)[0]))
#define CHECK(CND,LBL) {if(!(CND)){goto LBL;}}

int show_menu(const char *pcTitle, int iMenuStyle, int iDefaultItem, int iMaxItems, const char *tpcItems[]);
void display_small (byte ucLine, byte ucColumn, char *pcText);
void display_medium (byte ucLine, byte ucColumn, char *pcText);
void display_large (byte ucLine, byte ucColumn, char *pcText);
int timer_start(byte ucTimerNbr, int iDelay);
int timer_get(byte ucTimerNbr);
int timer_stop(byte ucTimerNbr);