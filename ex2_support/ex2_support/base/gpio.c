#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

//====== Function to set up GPIO mode and interrupts ======//

void
setupGPIO ()
{
  *CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_GPIO;	//Enable GPIO clock
  *GPIO_PA_CTRL = 2;		//Set high drive strength
  *GPIO_PA_MODEH = 0x55555555;	//Set pins A8-15 (LEDs) as output
  *GPIO_PA_DOUT = 0xff00;	//Turn off LEDs D4-D8 (LEDs are active low)
  *GPIO_PC_MODEL = 0x33333333;	//Set pins D0-7 (BTNs) as input
  *GPIO_PC_DOUT = 0xff;		//Enables pullup resistors for BTNs
}
