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
		// switch the 13 bit of the CMU register to 1 to enable clock//
		  // load CMU base address
		  ldr r1, =CMU_BASE
		  
		  //load current value of HFPERCLKEN ENABLE
		  ldr r2,[r1,#CMU_HFPERCLKEN0]
		  mov r3,#1
		  lsl r3,r3,#13
		  orr r2,r2,r3
		  str r2, [r1,#CMU_HFPERCLKEN0]
		  //FINISH OF FIRST STEP
		  
		  // now with GPIO_CTRL
		  ldr r1, =GPIO_PA_BASE
		  mov r3,#1   //MOVE VALUE 2 TO R3
		  str r3, [r1,#GPIO_CTRL] //STORE VALUE 2 TO GPIO_PA_CTRL
		  
		  //now with GPI_PA_MODEH
		  ldr r1, =GPIO_PA_BASE
		  ldr r3,=0x55555555   //MOVE VALUE 5555555 TO R3
		  str r3, [r1,#GPIO_MODEH] //STORE VALUE 5555555 TO GPIO_MODEH
		  
		  //now with GPIO_PA_DOUT
		  
		  ldr r1, =GPIO_PA_BASE
		  ldr r3,=0x0f00   //MOVE VALUE f0 TO R3
		  str r3, [r1,#GPIO_DOUT] //STORE VALUE f0 TO GPIO_DOUT
		  
		  //now with GPIO_PC_MODEL
		  ldr r1, =GPIO_PC_BASE
		  ldr r3,=0x33333333   //MOVE VALUE 3333333 TO R3
		  str r3, [r1,#GPIO_MODEL] //STORE VALUE 333333 TO GPIO_PC_MODEL
		  
		  //now with GPIO_PC_DOUT
		  ldr r1, =GPIO_PC_BASE
		  ldr r3,=0xff   //MOVE VALUE ff TO R3
		  str r3, [r1,#GPIO_DOUT] //STORE VALUE ff TO GPIO_PC_DOUT
		  
		  
		  //now reading from GPIO_PC_DIN
		  
		  ldr r1, =GPIO_PC_BASE
		 loop1:
		  ldr r2, [r1,#GPIO_DIN] //load the value to r2
		  lsl r2,r2,#8
		  ldr r3, =GPIO_PA_BASE
		  str r2, [r3,#GPIO_DOUT]
	      b loop1
	      
	
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
        
       
