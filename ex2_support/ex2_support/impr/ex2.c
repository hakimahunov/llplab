#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"		//File with register definitions
#include "common.h"		//For self defined variables and functions


int main(void)
{
    //====== PERIFERALS INITIALIZATON ======//
    setupGPIO();
    //setupDAC();                               //Migrated to interrupt_handler.c
    setupTimer(SAMPLE_PERIOD);
    setupNVIC();

    energyOptimizationRoutine();
    sleepMode(6);

    //====== WAIT FOR INTERRUPT ======// 
    while (1) {
	__asm__("wfi");
    }

    return 0;
}

/**
 * This function enables the deep sleep mode.
 * 
 * Arguments: mode (value which the energy mode)
 * Return: None
 * */
void sleepMode(int mode)
{
    *SCR = mode;
}

/**
 * This function is in charge of the energy optimization setup. It sets
 * the LEDs drive strength, disables RAM block and the instruction chache.
 * 
 * Arguments: None
 * Return: None
 * */
void energyOptimizationRoutine()
{
    *GPIO_PA_CTRL = 0x1;	//Set high drive strength for LEDs
    *EMU_MEMCTRL = 0x3;		//Disable RAM blocks 1-2
    *MSC_READCTRL |= (1 << 3);	//Disable instruction cache
}
