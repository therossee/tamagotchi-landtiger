#ifndef ADC_H
#define ADC_H

#include <string.h>
#include "LPC17xx.h"

/* lib_adc.c */
void ADC_init (void);
void ADC_start_conversion (void);

/* IRQ_adc.c */
void ADC_IRQHandler(void);
unsigned short getVolume(void);

#endif
