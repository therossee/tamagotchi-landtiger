/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include "lpc17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../TouchPanel/TouchPanel.h"
#include "../tamagotchi.h"
#include "../music/music.h"
#include "../adc/adc.h"
#include <stdio.h>

uint8_t frame = 1;
uint8_t last_frame;
extern uint8_t ss;
extern uint8_t mm;
extern uint8_t hh;
extern uint8_t happiness;
extern uint8_t satiety;
extern uint8_t count_satiety;
extern uint8_t count_happiness;
extern uint8_t direction;
extern uint16_t curr_posX;
extern uint8_t gameover;
extern uint8_t status;
int count_anim;
extern uint8_t flag;
volatile uint8_t track;
uint8_t count_cuddles = 0;

uint16_t SinTable[45] =                                       /* ÕýÏÒ±í                       */
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**	it handles the age counter and happiness/satiety counters
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void TIMER0_IRQHandler (void)
{
	char str[50];
		if(ss<59){
				ss++;
		}else if (ss == 59 && mm < 59){
					ss=0;
					mm++;
		}else if (ss == 59 && mm == 59){
					ss=0;
					mm=0;
					hh++;
		}
		sprintf(str, "%02d:%02d:%02d", hh, mm, ss);
		GUI_Text(38, 3, (uint8_t*)str, Black, White);
		if(status != 2 && status != 3)
			count_happiness--;
		if(status != 1 )
			count_satiety--;
		if( count_happiness==0 ){
			count_happiness=5;
			happiness--;
			delete_lives(0, happiness);		
		}
		if( count_satiety == 0 ){
			count_satiety=5;
			satiety--;
			delete_lives(1, satiety);
		}
		if( happiness == 0 ){
				gameover=1;
				Tamagotchi_gameover();
		}
		if( satiety == 0 && !gameover ){
				gameover=1;
				Tamagotchi_gameover();
		}
		LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
**		it handles all the animations and the ADC if the RIT is handling
**		a selected action
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void TIMER1_IRQHandler (void)
{
	if(!check_status()){
		ADC_start_conversion();
	}
	
	if( gameover ){
		go_away_anim();
		if( curr_posX >= 240 )
			flag = 1;
	}
	if(status == 3){
		cuddles_animation();
		count_cuddles++;
		LPC_TIM1->IR = 1;
		return;
	}
	if(direction == 0){
		draw_character(curr_posX, 105, frame);
	}else if ( direction == 1 ){
			go_left_anim();
			count_anim--;
			if ( count_anim == 0 )
				flag = 0;
	}else if ( direction == 2 ){
			go_right_anim();
			count_anim++;
			if(count_anim == 8)
				flag = 1;
	}
	if(frame == 0){
			last_frame=frame;
			frame++;
	}else if(frame == 1){
		if(last_frame == 0){
			frame++;
		}else{
			frame--;
		}
	}else if(frame == 2){
		last_frame=frame;
		frame--;
	}
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

/* Timers 2 and 3 handle the music playing */

void TIMER2_IRQHandler (void)
{
	static int sineticks=0;
	/* DAC management */	
	static int currentValue;
	currentValue = (uint16_t)((float)SinTable[sineticks]*getVolume()/1024*1.25);					//all the SinTable values are multiplied by 1.25 to reach the full scale 1024 peak value
	if(currentValue < 32){
		currentValue = 0;
	}
	LPC_DAC->DACR = (currentValue<<6);
	sineticks++;
	if(sineticks==45) sineticks=0;

	
  LPC_TIM2->IR = 1;			/* clear interrupt flag */
  return;
}

void TIMER3_IRQHandler (void)
{
	disable_timer(2);
  LPC_TIM3->IR = 1;			/* clear interrupt flag */
  return;
}
/******************************************************************************
**                            End Of File
******************************************************************************/
