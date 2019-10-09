#include <stdint.h>
#include <stdbool.h>
#include "efm32gg.h"

/**
 * This function initializes the timer and sets the timer period.
 * 
 * Arguments: period (period of the timer)
 * Return: None
 * */
void setupTimer(uint16_t period)
{
    uint32_t timerEnable = 1 << 6;
    *CMU_HFPERCLKEN0 |= timerEnable;	//Enable clock for the timer
    *TIMER1_TOP = period;	//Write the period for the timer
}

/**
 * This function starts the timer.
 * 
 * Arguments: period (Period of the timer)
 * Return: None
 * */
void releaseTimer()
{
    *TIMER1_IEN = 1;		//Enable interrupt for timer
    *TIMER1_CMD = 1;		//Start the timer
}

/**
 * This function suspends the timer.
 * 
 * Arguments: period (Period of the timer)
 * Return: None
 * */
void suspendTimer()
{
    *TIMER1_IEN = 0;		//Disable interrupt for timer
    *TIMER1_CMD = 2;		//Stop the timer
}
