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

	while(1); //wait for interrupts

    return 0;
}

void btnHandler(int btnState)
{
    bool isExplosion = false;	//Is needed for adjusting period duration for the notes
    if (BTN_CHECKER(4, btnState)) {	// SW5
	playSound(pinkPanther, pinkPantherCoefs,
		  sizeof(pinkPanther) / sizeof(int), isExplosion);
    } else if (BTN_CHECKER(5, btnState)) {	// SW6
	playSound(blaster, blasterCoefs,
		  sizeof(blaster) / sizeof(int), isExplosion);
    } else if (BTN_CHECKER(6, btnState)) {	// SW7
	playSound(blasterStrong, blasterStrongCoefs,
		  sizeof(blasterStrong) / sizeof(int), isExplosion);
    } else if (BTN_CHECKER(7, btnState)) {	// SW8
    isExplosion = true;
	playSound(explosion, explosionCoefs,
		  sizeof(explosion) / sizeof(int), isExplosion);
    }
}
