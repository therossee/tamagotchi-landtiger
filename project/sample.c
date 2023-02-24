/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "GLCD/GLCD.h" 
#include "RIT/RIT.h"
#include "timer/timer.h"
#include "tamagotchi.h"
#include "joystick/joystick.h"
#include "TouchPanel/TouchPanel.h"
#include "adc/adc.h"
#include "music/music.h"



#define SIMULATOR 1

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif


void speaker_init(void);

int main(void)
{
  SystemInit();  												/* System Initialization (i.e., PLL)  */
  LCD_Initialization();
	TP_Init();
	joystick_init();											/*	Timers and RIT priorities set, ADC, TP, joystick and speaker inits */
	NVIC_SetPriority(TIMER0_IRQn, 2);
	NVIC_SetPriority(TIMER1_IRQn, 1);
	NVIC_SetPriority(RIT_IRQn, 3);
	NVIC_SetPriority(TIMER2_IRQn, 0);
	NVIC_SetPriority(TIMER3_IRQn, 0);
	ADC_init();
	speaker_init();
	TouchPanel_Calibrate();
	Tamagotchi_init();
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
	
  while (1)	
  {
		__ASM("wfi");
  }
}

void speaker_init(void){
	LPC_PINCON->PINSEL1 |= (1<<21);
	LPC_PINCON->PINSEL1 &= ~(1<<20);
	LPC_GPIO0->FIODIR |= (1<<26);
}



/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
