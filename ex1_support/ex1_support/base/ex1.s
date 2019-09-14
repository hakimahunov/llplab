        .syntax unified
	
	      .include "efm32gg.s"

	/////////////////////////////////////////////////////////////////////////////
	//
	// Exception vector table
	// This table contains addresses for all exception handlers
	//
	/////////////////////////////////////////////////////////////////////////////
	
        .section .vectors
	
	      .long   stack_top               /* Top of Stack                 */
	      .long   _reset                  /* Reset Handler                */
	      .long   dummy_handler           /* NMI Handler                  */
	      .long   dummy_handler           /* Hard Fault Handler           */
	      .long   dummy_handler           /* MPU Fault Handler            */
	      .long   dummy_handler           /* Bus Fault Handler            */
	      .long   dummy_handler           /* Usage Fault Handler          */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* SVCall Handler               */
	      .long   dummy_handler           /* Debug Monitor Handler        */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* PendSV Handler               */
	      .long   dummy_handler           /* SysTick Handler              */

	      /* External Interrupts */
	      .long   dummy_handler
	      .long   gpio_handler            /* GPIO even handler */
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   gpio_handler            /* GPIO odd handler */
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler

	    .section .text

	/////////////////////////////////////////////////////////////////////////////
	//
	// Reset handler
	// The CPU will start executing here after a reset
	//
	/////////////////////////////////////////////////////////////////////////////

	    .globl  _reset
	    .type   _reset, %function
        .thumb_func
_reset: 

		ldr r8, =GPIO_PA_BASE
		ldr r9, =GPIO_PC_BASE
		
		//====== ENABLE CMU FOR GPIO ======//
		
		ldr r0, =CMU_BASE					//Load base addresse of CMU
		ldr r1, [r0, #CMU_HFPERCLKEN0]		//Load content of HFPERCLKEN register
		mov r2, #1
		lsl r2, r2, #CMU_HFPERCLKEN0_GPIO	//Shift left to set 13th bit
		orr r2, r2, r1						//Set 13th bit wrt content of HFPERCLKEN register
		str r2, [r0, #CMU_HFPERCLKEN0]		//Store the new content to HFPERCLKEN register
		
		//====== SETUP GPIO PINS =====//
		
		mov r1, #0x2
		str r1, [r8, #GPIO_CTRL]			//Set high drive strength
		
		ldr r1, =0x55555555
		str r1, [r8, #GPIO_MODEH]			//Set pins 8-15 (LEDs) to output 
		
		ldr r1, =0x33333333
		str r1, [r9, #GPIO_MODEL]			//Set pins 0-7 (Buttons) to input
		ldr r1, =0xff
		str r1, [r9, #GPIO_DOUT]			//Enable internal pull-up
		
		//====== TURN ON AND OFF LEDS ======//
		
		ldr r1, =0x0						//Value to write to LEDs
		lsl r1, r1, #8						//Shift 8 bits to from least significant to most significant
		str r1, [r8, #GPIO_DOUT]			//Write value to LEDs
		ldr r2, =0x7a1200					//Counter capacity
		loop:
			sub r2, r2, #1					//Decrement counter
			cmp r2, #0						//Compare counter
			bne loop						//If not equal to 0 goto loop
		ldr r1, =0xff						//Else turn off LEDs
		lsl r1, r1, #8
		str r1, [r8, #GPIO_DOUT]
		
		
		//====== POLL ======//
		
		poll:
			ldr r1, [r9, #GPIO_DIN]			//Read value of Buttons
			lsl r1, r1, #8					//Shift 8 bits to from least significant to most significant
			str r1, [r8, #GPIO_DOUT]		//Write Buttons value to LEDs
			b poll							//Polling infinitely
		
		
		
		
	    b .  // do nothing
	
	/////////////////////////////////////////////////////////////////////////////
	//
	// GPIO handler
	// The CPU will jump here when there is a GPIO interrupt
	//
	/////////////////////////////////////////////////////////////////////////////
	
        .thumb_func
gpio_handler:  

		b .  // do nothing
	
	/////////////////////////////////////////////////////////////////////////////
	
        .thumb_func
dummy_handler:  
		b .  // do nothing

