#include <stdint.h>
#include <stdbool.h>
#include "efm32gg.h"

void
setupDAC ()
{
  *CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_DAC0;	//Enable clock for DAC
  uint32_t dacPrescaler = 0x50010;	//Select prescaler for DAC
  *DAC0_CTRL = dacPrescaler;
  *DAC0_CH0CTRL = 1;		//Enable left channel for DAC
  *DAC0_CH1CTRL = 1;		//Enable right channel for DAC
}

void
pushToDAC (int sample)
{
  *DAC0_CH0DATA = sample;
  *DAC0_CH1DATA = sample;
}