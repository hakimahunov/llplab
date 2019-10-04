#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "common.h"



int main(void)
{
	//====== Call the peripheral setup functions ======//

	setupGPIO();
	setupDAC();
	setupTimer(SAMPLE_PERIOD);

	playSound(blaster, blasterCoefs, sizeof(blaster) / sizeof(int));

	while (1) {
		int btnState = *GPIO_PC_DIN;
		*GPIO_PA_DOUT = btnState << 8;


	}

	return 0;
}
