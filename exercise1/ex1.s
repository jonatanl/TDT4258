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
      	// load CMU base address
      	ldr r1, =CMU_BASE
      
      	// load current value of HFPERCLK_ENABLE
      	ldr r2, [r1, #CMU_HFPERCLKEN0]
      
      	// set bit for GPIO clk
      	mov r3, #1
      	lsl r3, r3, #CMU_HFPERCLKEN0_GPIO
      	orr r2, r2, r3
      
      	//store new value
      	str r2, [r1, #CMU_HFPERCLKEN0]

        // set drive strength to high (0x2)
        ldr r1, =GPIO_PA_BASE + GPIO_CTRL
        mov r2, #0x2
      	str r2, [r1]
        
        // Set pin 8-15 output
        ldr r1, =GPIO_PA_BASE + GPIO_MODEH
        mov r2, #0x55
        orr r2, r2, r2, lsl #8
        orr r2, r2, r2, lsl #16
        str r2, [r1]

        // Set 8-15 high
        ldr r1, =GPIO_PA_BASE + GPIO_DOUT
        mov r2, #0x1
        lsl r2, r2, #8
        str r2, [r1]

	// Set pins 0-7 to input
        ldr r1, =GPIO_PC_BASE + GPIO_MODEL
        mov r2, #0x33
        orr r2, r2, r2, lsl #8
        orr r2, r2, r2, lsl #16
        str r2, [r1]

	// Enable pull-up
        ldr r1, =GPIO_PC_BASE + GPIO_DOUT
        mov r2, #0xff
        str r2, [r1]

enable_interrupt:
	ldr r1, =GPIO_BASE
	ldr r2, =0x22222222
	str r2, [r1, #GPIO_EXTIPSELL]

	// Set interrupt on 1->0
	ldr r2, =0x0ff
	str r2, [r1, #GPIO_EXTIFALL]

	// Set interrupt on 0->1
	str r2, [r1, #GPIO_EXTIRISE]

	// Enable interrupt generation
	str r2, [r1, #GPIO_IEN]
	
	// Enable interrupt handling
	ldr r1, =ISER0
	ldr r2, =0x802
	str r2, [r1] 

enable_sleep: 
	// Enable deep sleep mode 2
	ldr r1, =SCR
	mov r2, #6
	str r2, [r1]

program_loop:
	// Go to sleep
 	wfi
	b program_loop  
  /////////////////////////////////////////////////////////////////////////////
  //
  // GPIO handler
  // The CPU will jump here when there is a GPIO interrupt
  //
  /////////////////////////////////////////////////////////////////////////////
  
        .thumb_func
gpio_handler:  
        ldr r1, =GPIO_BASE
	ldr r2, [r1, #GPIO_IF]
	str r2, [r1, #GPIO_IFC]
	
	// Write button input to leds
	ldr r3, =GPIO_PA_BASE + GPIO_DOUT
	lsl r2, r2, #8
	mvn r2, r2
	str r2, [r3]
	
	bx lr

  /////////////////////////////////////////////////////////////////////////////
  
        .thumb_func
dummy_handler:  
        b .  // do nothing

