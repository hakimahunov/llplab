#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "common.h"



int main(void)
{
    //====== PERIFERALS INITIALIZATON ======//

    setupGPIO();
    setupDAC();
    setupTimer(SAMPLE_PERIOD);
    setupNVIC();

    //====== WAITING FOR INTERRUPT ======// 

    while (1) {}

    return 0;
}
