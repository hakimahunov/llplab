#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

#define   SAMPLE_PERIOD   0

//====== Declaration of peripheral setup functions ======//

void setupGPIO();
//void setupTimer (uint32_t period);
//void setupDAC ();
//void setupNVIC ();

int
main (void)
{
  //====== Call the peripheral setup functions ======//

  setupGPIO ();
  //setupDAC();
  //setupTimer(SAMPLE_PERIOD);

  while (1)
    {
      int btnState = *GPIO_PC_DIN;
      *GPIO_PA_DOUT = btnState << 8;
    }

  return 0;
}
