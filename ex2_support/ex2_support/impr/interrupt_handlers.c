#include <stdint.h>
#include <stdbool.h>
#include "common.h"
#include "efm32gg.h"

/*
 * TIMER1 interrupt handler 
 */
void __attribute__ ((interrupt)) TIMER1_IRQHandler()
{
	/*
	 * TODO feed new samples to the DAC remember to clear the pending
	 * interrupt by writing 1 to TIMER1_IFC 
	 */
}

/*
 * GPIO even pin interrupt handler 
 */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler()
{
  
	int btnState = *GPIO_PC_DIN;	//Read the buttons state
	*GPIO_PA_DOUT = btnState << 8;	//Write buttons state to LEDs
	    btnHandler(btnState);	//If any button pressed, then handle it
	     *GPIO_IFC = *GPIO_IF ; //Clear interrupt
    *GPIO_PA_DOUT =0xff00;
}

/*
 * GPIO odd pin interrupt handler 
 */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler()
{
    
	int btnState = *GPIO_PC_DIN;	//Read the buttons state
	*GPIO_PA_DOUT = btnState << 8;	//Write buttons state to LEDs
	 btnHandler(btnState);	//If any button pressed, then handle it
     *GPIO_IFC = *GPIO_IF ; //Clear interrupt
     *GPIO_PA_DOUT =0xff00;
}
