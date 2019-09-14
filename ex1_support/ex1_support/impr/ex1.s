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
		//====== ENABLE CMU FOR GPIO ======//
		
		ldr r0, =CMU_BASE					//Load base addresse of CMU
		ldr r1, [r0, CMU_HFPERCLKEN0]		//Load content of HFPERCLKEN register
		mov r2, #1
		lsl r2, r2, #CMU_HFPERCLKEN0_GPIO	//Shift left to set 13th bit
		orr r2, r2, r1						//Set 13th bit wrt content of HFPERCLKEN register
		str r2, [r0, CMU_HFPERCLKEN0]		//Store the new content to HFPERCLKEN register
		
		//====== SETUP GPIO PINS =====//
		
		ldr r0, =GPIO_PA_BASE				//Load base addresse of GPIO_PA
		mov r1, #0x2
		str r1, [r0, GPIO_CTRL]				//Set high drive strength
		
		ldr r1, =0x55555555
		srt r1, [r0, GPIO_MODEH]			//Set pins 8-15 (LEDs) to output 
		
		ldr r0, =GPIO_PC_BASE				//Load base addresse of GPIO_PA
		ldr r1, =0x33333333
		srt r1, [r0, GPIO_MODEL]			//Set pins 0-7 (Buttons) to input
		ldr r1, =0xff
		srt r1, [r0, GPIO_DOUT]				//Enable internal pull-up
		
		ldr r0, =GPIO_PA_BASE
		ldr r1, =0xff
		lsl r1, r1, #8
		srt r1, [r0, GPIO_DOUT]

		
		
		
		
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

