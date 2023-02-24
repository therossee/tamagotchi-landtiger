#ifndef __Tamagotchi_H
#define __Tamagotchi_H

#include <string.h>
#include "GLCD/GLCD.h"

void Tamagotchi_init(void);
void Tamagotchi_setCounter(void);
void Tamagotchi_eatMeal(void);
void Tamagotchi_eatSnack(void);
void Tamagotchi_cuddles(void);
void highlight_action(uint8_t selected_action);
void reset_action(uint8_t unselected_action);
void Tamagotchi_gameover(void);
uint8_t check_exit(void);
uint8_t check_gameover(void);
uint8_t check_status(void);
void playMeal(void);
void playSnack(void);
void playOver(void);
void playCuddles(void);
uint8_t gotoGameover(void);
uint8_t check_exit_gameover(void);
uint8_t checkAreaTP(void);
void playInit(void);

#endif
