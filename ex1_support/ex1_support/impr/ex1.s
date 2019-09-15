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

		ldr r8, =GPIO_PA_BASE				//Register for LEDs
		ldr r9, =GPIO_PC_BASE				//Register for Buttons
		ldr r10, =GPIO_BASE					//Register for GPIOs
		
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
		mov r1, 0xff
		str r1, [r9, #GPIO_DOUT]			//Enable internal pull-up
		
		//====== TURN ON AND OFF LEDS ======//
		
		ldr r1, =0x0						//Value to write to LEDs
		lsl r1, r1, #8						//Shift 8 bits to from least significant to most significant
		str r1, [r8, #GPIO_DOUT]			//Write value to LEDs
		ldr r2, =0x7a1200					//Counter capacity
		loop:
			sub r2, r2, #1					//Decrement counter
			cmp r2, #0						//Compare counter
			bne loop						//If is not equal to 0 goto loop
		mov r1, 0xff						//Else turn off LEDs
		lsl r1, r1, #8
		str r1, [r8, #GPIO_DOUT]
		
		//====== DISABLE INSTRUCTION CACHE ======//
		
		ldr r5, =MSC
		ldr r6, [r5, #MSC_READ_CONTROL]
		
		mov r4, #1
		lsl r4, r4, #3
		orr r6, r6, r4
		
		str r6, [r5, #MSC_READ_CONTROL]
		
		//====== DISABLE RAM BLOCK ======//
	
		ldr r5, =EMU_BASE
		mov r6, #0x7
		str r6, [r5, #EMU_MEMCTRL]
		
		
		//====== ENABLES DEEP SLEEP ======//
		ldr r5, =SCR						
		mov r6, #6							
		str r6, [r5]						
		
		//====== ENABLE INTERRUPT FOR GPIO ======//
	
		ldr r1, [r10, #GPIO_IF]				
		str r1, [r10, #GPIO_IFC]				
		
		ldr r1, =0x22222222					
		str r1, [r10, #GPIO_EXTIPSELL]
		
		mov r1, #0xff
		str r1, [r10, #GPIO_EXTIFALL]		
		//str r1, [r10, #GPIO_EXTIRISE]		
		str r1, [r10, #GPIO_IEN]				
		
		ldr r2, =ISER0						
		ldr r1, =0x802						
		str r1, [r2]						
		
		wfi
		
	
	/////////////////////////////////////////////////////////////////////////////
	//
	// GPIO handler
	// The CPU will jump here when there is a GPIO interrupt
	//
	/////////////////////////////////////////////////////////////////////////////
	
        .thumb_func
gpio_handler:  
		ldr r1, [r10, #GPIO_IF]				
		str r1, [r10, #GPIO_IFC]				
		
		ldr r0, [r9, #GPIO_DIN]			//Read state of Buttons
		
		cmp r0, #0b11111110				//Check if Left bnt pressed
		beq shift_left
		cmp r0, #0b11111011				//Check if Right bnt pressed
		beq shift_right
		cmp r0, #0b11111101				//Check if Up bnt pressed
		beq vol_up
		cmp r0, #0b11110111				//Check if Down bnt pressed
		beq vol_down
		
		bx lr							//Return from interrupt
		
		
		
		shift_left:
			ldr r2, [r8, #GPIO_DIN]			//Load LEDs state
			lsr r2, r2, #8					//Shift 8 bits to from most significant to least significant
			lsr r1, r2, #1					//Shift right 1 bit
			add r1, r1, #0x80				//Turn off one LED from the left
			lsl r1, r1, #8
			str r1, [r8, #GPIO_DOUT]		//Write shifted state to LEDs
			bx lr
		shift_right:
			ldr r2, [r8, #GPIO_DIN]			//Load LEDs state
			lsr r2, r2, #8					//Shift 8 bits to from most significant to least significant
			lsl r1, r2, #1					//Shift left 1 bit
			add r1, r1, #0x01				//Turn off one LED from the right
			lsl r1, r1, #8
			str r1, [r8, #GPIO_DOUT]		//Write shifted state to LEDs
			bx lr
		vol_up:
			ldr r2, [r8, #GPIO_DIN]			//Load LEDs state
			ror r1, r2, #1					//Rotate 1 bit right
			str r1, [r8, #GPIO_DOUT]		//Write shifted state to LEDs
			bx lr
		vol_down:
			ldr r2, [r8, #GPIO_DIN]			//Load LEDs state
			lsl r1, r2, #1					//Shift left 1 bit
			str r1, [r8, #GPIO_DOUT]		//Write shifted state to LEDs
			bx lr
	
	/////////////////////////////////////////////////////////////////////////////
	
        .thumb_func
dummy_handler:  
		b .  // do nothing

