#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "common.h"



int main(void)
{
    //====== PERIFERALS INITIALIZATON ======//

    setupGPIO();
    //setupDAC(); //Migrated to interrupt_handler.c
    setupTimer(SAMPLE_PERIOD);
    setupNVIC();
    
    energyOptimizationRoutine();
    
    sleepMode(6);

    //====== WAITING FOR INTERRUPT ======// 

    while (1) {
    	__asm__("wfi");
    }

    return 0;
}

void sleepMode(int mode)
{
	*SCR = mode;
}

void energyOptimizationRoutine()
{
	*GPIO_PA_CTRL = 0x1; //Set high drive strength for LEDs

	*EMU_MEMCTRL = 0x3; //Disable RAM blocks 1-2
    
    *MSC_READCTRL |= (1 << 3); //Disable instruction cache
    
}
