#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

#define   SAMPLE_PERIOD   0

//====== Declaration of peripheral setup functions ======//

void setupGPIO ();
void setupTimer (uint32_t period);
void setupDAC ();
void pushToDAC (int sample);

int
main (void)
{
  //====== Call the peripheral setup functions ======//

  setupGPIO ();
  setupDAC();
  setupTimer(SAMPLE_PERIOD);
	int t = 0;
	int s = 200;
  while (1)
    {
      int btnState = *GPIO_PC_DIN;
      *GPIO_PA_DOUT = btnState << 8;
      if (t < 10000) 
      {
      	pushToDAC(s);	
      } else
      {
      	t = 0;
      	if (s > 4000) s = 200; else s = s + 100;
      }
    }

  return 0;
}
