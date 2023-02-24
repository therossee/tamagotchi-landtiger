/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "../tamagotchi.h"
#include "../TouchPanel/TouchPanel.h"
#include "../music/music.h"
#include "../timer/timer.h"
#include "../adc/adc.h"


extern uint8_t game_over;
/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
// beat 1/4 = 1.65/4 seconds
#define RIT_SEMIMINIMA 8
#define RIT_MINIMA 16
#define RIT_INTERA 32

#define UPTICKS 1

//SHORTENING UNDERTALE: TOO MANY REPETITIONS

NOTE click[] = 
{
	{g4, time_biscroma},
	{e5, time_semiminima},
	{pause, time_semicroma}
};

void playClick(void);

extern uint8_t current_action;    //0 means no selection, 1 means left (meal), 2 means right (snack), 3 means reset
extern uint8_t init;

void RIT_IRQHandler (void)
{					
	static int select=0;
	static int left=0;
	static int right=0;
	static int tp = 0;
	
	ADC_start_conversion();
	
	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){      /* select pressed */
			select++;
		switch(select){
			case 1:
			if(current_action == 0){
					break;
			}else if(current_action == 1){
				playClick();
				if(check_gameover())
					break;
				reset_action(current_action);
				current_action=0;
				Tamagotchi_eatMeal();
			}else if (current_action == 2){
				playClick();
				if(check_gameover())
					break;
				reset_action(current_action);
				current_action=0;
				Tamagotchi_eatSnack();
			}else{
				playClick();
				Tamagotchi_init();
			}
				break;
			default:
				break;
			}
		}else	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){				//left triggered
		left++;
		switch(left){
			case 1:
				if(current_action == 3){
					break;
				}
				if(current_action == 2){
					reset_action(current_action);
				}
				current_action = 1;
				highlight_action(current_action);
				disable_RIT();
				reset_RIT();
				enable_RIT();
				break;
			default:
				break;
			}
		}else if ((LPC_GPIO1->FIOPIN & (1<<28)) == 0){			//right triggered
		right++;
		switch(right){
			case 1:
				if(current_action == 3){
					break;
				}
				else{
					if(current_action == 1){
						reset_action(current_action);
					}
					current_action = 2;
					highlight_action(current_action);
				}
				break;
			default:
				break;
		}
	}else if(getDisplayPoint(&display, Read_Ads7846(), &matrix)){       //touchscreen 
		tp++;
		switch(tp){
			case 1:
				if(checkAreaTP()){
					Tamagotchi_cuddles();
				}
				else
					break;
				break;
			default:
				break;
		}			
	}
	else{
		select=0;
		right=0;
		left=0;
		tp=0;	
	}
	disable_RIT();
	reset_RIT();
	enable_RIT();
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
  return;
}

/* only the click sound is played directly on RIT */

void playClick(void){
	static int currentNote = 0;
	static int ticks = 0;	
	uint16_t noteNumber = (sizeof(click)/sizeof(click[0]));
	while(currentNote < noteNumber){
		if(!isNotePlaying()){
				++ticks;					
			if(ticks == UPTICKS)
			{
				ticks = 0;
				playNote(click[currentNote++]);
			}
		}				
	}
	currentNote=0;
}




/******************************************************************************
**                            End Of File
******************************************************************************/
