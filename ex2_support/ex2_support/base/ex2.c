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

    //====== BUSY-WAITING FOR BUTTON CLICK ======// 

    while (1) {
	int btnState = *GPIO_PC_DIN;	//Read the buttons state
	*GPIO_PA_DOUT = btnState << 8;	//Write buttons state to LEDs
	if (btnState != 0xff)
	    btnHandler(btnState);	//If any button pressed, then handle it
    }

    return 0;
}

void btnHandler(int btnState)
{
    bool isExplosion = false;	//Is needed for adjusting period duration for the notes
    switch (btnState) {
    case 0xef:			//SW5
	playSound(pinkPanther, pinkPantherCoefs,
		  sizeof(pinkPanther) / sizeof(int), isExplosion);
	break;
    case 0xdf:			//SW6
	playSound(blaster, blasterCoefs,
		  sizeof(blaster) / sizeof(int), isExplosion);
	break;
    case 0xbf:			//SW7
	playSound(blasterStrong, blasterStrongCoefs,
		  sizeof(blasterStrong) / sizeof(int), isExplosion);
	break;
    case 0x7f:			//SW8
	isExplosion = true;
	playSound(explosion, explosionCoefs,
		  sizeof(explosion) / sizeof(int), isExplosion);
	break;
    default:
	break;
    }
}
