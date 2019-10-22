#include <stdint.h>
#include <stdbool.h>
#include "efm32gg.h"

void setupTimer(uint16_t period)
{
    uint32_t timerEnable = 1 << 6;
    *CMU_HFPERCLKEN0 |= timerEnable;	//Enable clock for the timer
    *TIMER1_TOP = period;	//Write the period for the timer
}

void releaseTimer()
{
    *TIMER1_IEN = 1;		//Enable interrupt for timer
    *TIMER1_CMD = 1;		//Start the timer
}

void suspendTimer()
{
    *TIMER1_IEN = 0;		//Disable interrupt for timer
    *TIMER1_CMD = 2;		//Stop the timer
}
