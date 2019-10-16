#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/**
 * This function initializes the GPIOs. Buttons are set as inputs
 * and LEDs are sets as outputs. Interrupts are set for the buttons.
 * 
 * Arguments: None
 * Return: None
 * */
void setupGPIO()
{
    *CMU_HFPERCLKEN0 |= CMU2_HFPERCLKEN0_GPIO;	//Enable GPIO clock
    *GPIO_PA_CTRL = 2;		//Set high drive strength
    *GPIO_PA_MODEH = 0x55555555;	//Set pins A8-15 (LEDs) as output
    *GPIO_PA_DOUT = 0xff00;	//Turn off LEDs D4-D8 (LEDs are active low)
    *GPIO_PC_MODEL = 0x33333333;	//Set pins D0-7 (BTNs) as input
    *GPIO_PC_DOUT = 0xff;	//Enables pullup resistors for BTNs

    *GPIO_EXTIPSELL = 0x22222222;	//Select C port for interrupts
    *GPIO_EXTIFALL = 0xFF;	//Enable falling edge interrupts
    *GPIO_IFC = *GPIO_IF;	//Clear interrupt
    *GPIO_IEN = 0xFF;		//Enables interrupts for GPIO
}

/**
 * This function enables interrupts
 * 
 * Arguments: None
 * Return: None
 * */
void setupNVIC()
{
    *ISER0 = 0x1802;
}
