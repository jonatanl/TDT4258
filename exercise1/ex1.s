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
      	ldr r1, cmu_base_addr
      
      	// load current value of HFPERCLK_ENABLE
      	ldr r2, [r1, #CMU_HFPERCLKEN0]
      
      	// set bit for GPIO clk
      	mov r3, #1
      	lsl r3, r3, #CMU_HFPERCLKEN0_GPIO
      	orr r2, r2, r3
      
      	//store new value
      	str r2, [r1, #CMU_HFPERCLKEN0]
        

        // set drive strength to high (0x2)
        ldr r1, gpio_pa_base
        mov r2, #0x2
      	str r2, [r1, #GPIO_CTRL]
        
        // Set pin 8-15 output
        ldr r1, gpio_pa_base
        mov r2, #0x55
        orr r2, r2, r2, lsl #8
        orr r2, r2, r2, lsl #16
        str r2, [r1, #GPIO_MODEH]

        // Set 8-15 high
        ldr r1, gpio_pa_base
        mov r2, #0x1
        lsl r2, r2, #8
        str r2, [r1, #GPIO_DOUT]

		// Set pins 0-7 to input
        ldr r1, gpio_pc_base
        mov r2, #0x33
        orr r2, r2, r2, lsl #8
        orr r2, r2, r2, lsl #16
        str r2, [r1, #GPIO_MODEL]

		// Enable pull-up
        ldr r1, gpio_pc_base
        mov r2, #0xff
        str r2, [r1, #GPIO_DOUT]


read_input:
		
		mov r1, gpio_pc_base
		add r1, r1, #GPIO_DIN

		mov r2, #0
		ldr r2, [r1]
		lsl r2, r2, #8

		ldr r3, gpio_pa_base
		str r2, [r3, #GPIO_DOUT]

		//b read_input 

  
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

cmu_base_addr:
	.long CMU_BASE        

gpio_pa_base:
        .long GPIO_PA_BASE

gpio_pc_base:
        .long GPIO_PC_BASE
