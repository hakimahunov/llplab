#include <stdint.h>
#include <stdbool.h>
#include "efm32gg.h"

/**
 * This function initializes the DAC. The frequency and the continous
 * mode is set by the prescaler, both channels are enabled.
 * 
 * Arguments: None
 * Return: None
 * */
void setupDAC()
{
    *CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_DAC0;	//Enable clock for DAC
    uint32_t dacPrescaler = 0x50010;	//Select prescaler for DAC
    *DAC0_CTRL = dacPrescaler;
    *DAC0_CH0CTRL = 1;		//Enable left channel for DAC
    *DAC0_CH1CTRL = 1;		//Enable right channel for DAC
}

/**
 * This function disables the DAC.
 * 
 * Arguments: None
 * Return: None
 * */
void disableDAC()
{
    *DAC0_CTRL = 0;
    *DAC0_CH0CTRL = 0;
    *DAC0_CH1CTRL = 0;
    *CMU_HFPERCLKEN0 &= ~CMU2_HFPERCLKEN0_DAC0;
}

/**
 * Write data to both DAC channels.
 * 
 * Arguments: sample (value to push into the DAC)
 * Return: None
 * */
void pushToDAC(int sample)
{
    *DAC0_CH0DATA = sample;
    *DAC0_CH1DATA = sample;
}
