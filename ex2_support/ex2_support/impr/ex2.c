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
