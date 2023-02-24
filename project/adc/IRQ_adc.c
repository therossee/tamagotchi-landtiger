/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_adc.c
** Last modified Date:  20184-12-30
** Last Version:        V1.00
** Descriptions:        functions to manage A/D interrupts
** Correlated files:    adc.h
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "lpc17xx.h"
#include "adc.h"
#include "../timer/timer.h"
#include "../GLCD/GLCD.h"

/*----------------------------------------------------------------------------
  A/D IRQ: Executed when A/D Conversion is ready (signal from ADC peripheral)
 *----------------------------------------------------------------------------*/

unsigned short AD_current;   
unsigned short AD_last = 0xFF;     /* Last converted value               */

uint8_t checkVolume(void);

/*it reads from ADC and updates ADC_current value if it has changed */
void ADC_IRQHandler(void) {
  	
  AD_current = ((LPC_ADC->ADGDR>>4) & 0xFFF);/* Read Conversion Result             */
  checkVolume();
	if(AD_current != AD_last){
		AD_last = AD_current;
	}
	
}

/* this function updates the volume icon based on ADC_current level*/

uint8_t checkVolume(void){
	static uint8_t ticks = 0;
	uint8_t levPrev = AD_last/1024;
	uint8_t levNow = AD_current/1024;
	if ( AD_current > 50 && levNow == 0){
		levNow = 1;
	}
	if ( AD_last > 50 && levPrev == 0 ){
		levPrev = 1;
	}
	if(levNow!=levPrev){
		draw_speaker(3, 62, levPrev); 
		draw_speaker(3, 62, levNow); 
		ticks=1;
	}
	else{
		if(ticks>0){
			ticks++;
			if(ticks>=40){
				draw_speaker(3, 62, levNow); 
				ticks = 0;
			}
		}
	}
	return levNow;
}


unsigned short getVolume(void){
	return AD_current/4;
}
