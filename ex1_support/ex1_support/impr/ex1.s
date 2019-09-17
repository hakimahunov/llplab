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


//INITIALIZATION

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
		  
		  //INIT for INTERRUPT PART STARTS HERE 
		  
		  //now with GPIO_EXTIPSELL
		  ldr r1, =GPIO_BASE
		  ldr r3,=0x22222222   //MOVE VALUE 22222222 TO R3
		  str r3, [r1,#GPIO_EXTIPSELL] //STORE VALUE 2222222 TO GPIO_EXTIPSELL
		  
		  //now with GPIO_EXTIFALL
		  ldr r1, =GPIO_BASE
		  ldr r3,=0xff   //MOVE VALUE ff TO R3
		  str r3, [r1,# GPIO_EXTIFALL] //STORE VALUE ff TO GPIO_EXTIFALL
		  
		  //now with GPIO_EXTIRISE
		  ldr r1, =GPIO_BASE
		  ldr r3,=0xff   //MOVE VALUE ff TO R3
		  str r3, [r1,# GPIO_EXTIRISE] //STORE VALUE ff TO GPIO_EXTIRISE
		  
		  //now with GPIO_IEN
		  ldr r1, =GPIO_BASE
		  ldr r3,=0xff   //MOVE VALUE ff TO R3
		  str r3, [r1,# GPIO_IEN] //STORE VALUE ff TO GPIO_IEN
		  
		  // ISER0
		  ldr r1, =ISER0
		  ldr r3,=0x802   //MOVE VALUE 802 TO R3
		  str r3, [r1] //STORE VALUE ff TO ISER0
		  
		  //power opt
		  ldr r1,=SCR
		  ldr r2,=0x6
		  str r2,[r1]
		  
		// waiting_loop:
		 wfi
		 	//b waiting_loop
		 	
	      
	
	/////////////////////////////////////////////////////////////////////////////
	//
  // GPIO handler
  // The CPU will jump here when there is a GPIO interrupt
	//
	/////////////////////////////////////////////////////////////////////////////
	
        .thumb_func
gpio_handler:  

		  //clear the interrupt 
		  
		  ldr r1, =GPIO_BASE
		  ldr r2,[r1,#GPIO_IF]
		  ldr r3,[r1,#GPIO_IFC]
		  mov r3,r2
		  
		  //now read from the interrupt, place it in output
		 
		  ldr r1, =GPIO_PC_BASE
		  ldr r2, [r1,#GPIO_DIN] //load the value to r2
		  lsl r2,r2,#8
		  ldr r3, =GPIO_PA_BASE
		  str r2, [r3,#GPIO_DOUT]
		  bx lr

	     // b .  // do nothing
	
	/////////////////////////////////////////////////////////////////////////////
	
        .thumb_func
dummy_handler:  
        b .  // do nothing
        
       

