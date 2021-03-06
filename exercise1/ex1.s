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
        .long   dummy_handler	//0
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
        .long   dummy_handler	//12
        .long   dummy_handler
        .long   dummy_handler
        .long   dummy_handler
        .long   dummy_handler
        .long   dummy_handler
        .long   dummy_handler
        .long   dummy_handler
        .long   dummy_handler	//20
        .long   dummy_handler
        .long   dummy_handler
        .long   dummy_handler
        .long   dummy_handler
        .long   dummy_handler
        .long   letimer0_handler	//26
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
        //Adding some aliases, assume _BASE if no other ID
        CMU .req R4
        GPIO_LED .req R5
        GPIO_btn .req R6
        GPIO .req R7
        EMU .req R8
        DMA .req R9
	LETIMER .req r10
	RTC .req r11
       
        ldr GPIO_LED, =GPIO_PA_BASE
        ldr GPIO_btn, =GPIO_PC_BASE
        ldr CMU, =CMU_BASE
        ldr EMU, =EMU_BASE
        ldr GPIO, =GPIO_BASE
        ldr DMA, =DMA_BASE
	ldr LETIMER, =LETIMER_BASE
	ldr RTC, =RTC_BASE
 
        // Enable clock for GPIO
	mov r1, #1
        lsl r1, r1, #CMU_HFPERCLKEN0_GPIO
        str r1, [CMU, #CMU_HFPERCLKEN0]

	// Enable LE interface clock
	mov r1, #0x10
	str r1, [CMU, #CMU_HFCORECLKEN0]
       
        // set drive strength to high (0x2)
        mov r1, #0x2
        str r1, [GPIO_LED, #GPIO_CTRL]
       
        // Set pin 8-15 output
        mov r1, #0x55
        orr r1, r1, r1, lsl #8
        orr r1, r1, r1, lsl #16
        str r1, [GPIO_LED, #GPIO_MODEH]
 
        // Set 8-15 high
        mov r1, #0x1
        lsl r1, r2, #8
        str r1, [GPIO_LED, #GPIO_DOUT]
 
        // Set pins 0-7 to input
        mov r1, #0x33
        orr r1, r1, r1, lsl #8
        orr r1, r1, r1, lsl #16
        str r1, [GPIO_btn, #GPIO_MODEL]
 
        // Enable pull-up
        mov r1, #0xff
        str r1, [GPIO_btn, #GPIO_DOUT]
 
        // Disable SRAM
        mov r1, #0x7
        str r1, [EMU, #EMU_MEM_CTRL]
 
        // Disabling LFACLK and LFBCLK
	// ULFRCO still enabled for LETIMER0
	// This also sets EM3
        mov r1, #0x1
	lsl r1, #0x10
        str r1, [CMU, #CMU_LFCLKSEL]

	// We enable ULFRCO for LETIMER and RTC
	mov r1, #6
	str r1, [CMU, #CMU_LFACLKEN0]
	
 
        // Make sure lights are off
        mov r1, #0xFF
        lsl r1, #0x8
        str r1, [GPIO_LED, #GPIO_DOUT]
 
       
enable_interrupt:
        ldr r1, =0x22222222
        str r1, [GPIO, #GPIO_EXTIPSELL]
 
        // Set interrupt on 1->0
        ldr r1, =0x0ff
        str r1, [GPIO, #GPIO_EXTIFALL]
 
        // Set interrupt on 0->1
        str r1, [GPIO, #GPIO_EXTIRISE]
 
        // Enable interrupt generation
        str r1, [GPIO, #GPIO_IEN]
       
        // Enable interrupt handling for GPIO odd, even and LETIMER0
       	mov r2, #1
	lsl r2, #26
	ldr r3, =0x802
	orr r2, r2, r3
        ldr r1, =ISER0
        str r2, [r1]

set_up_LETIMER:

	// Setting comp0 value, i.e which val we want to count down from
	ldr r1, =0x100
	str r1, [LETIMER, #LETIMER_COMP0]

	// We set the comp0 to be top val
	mov r1, #1
	lsl r1, #9
	str r1, [LETIMER, #LETIMER_CTRL]

	// We set the LETIMER to interrupt on underflow
	mov r1, #4
	str r1, [LETIMER, #LETIMER_IEN]
	// str r1, [LETIMER, #LETIMER_IFS]

	// We start LETIMER
	mov r1, #1
	str r1, [LETIMER, #LETIMER_CMD]
	
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
        ldr r1, [GPIO, #GPIO_IF]        // Reads which port has flagged interrupt
        str r1, [GPIO, #GPIO_IFC]       // Clears interrupt
	
        // Fetch current lights and buttons
        ldr r2, [GPIO_LED, #GPIO_DOUT]
        ldr r1, [GPIO_btn, #GPIO_DIN]
        // Align button input and LED output   
        lsl r1, r1, #8
        eor r1, r1, r2
        // XOR input and output
        mvn r1, r1
        str r1, [GPIO_LED, #GPIO_DOUT]

        bx lr

	

	.thumb_func
letimer0_handler:
	ldr r1, [LETIMER, #LETIMER_IF] 
	str r1, [LETIMER, #LETIMER_IFC]
	// Count down
	ldr r1, [GPIO_LED, #GPIO_DOUT]
	lsr r1, #8
	sub r1, #1
	lsl r1, #8
	str r1, [GPIO_LED, #GPIO_DOUT]

	bx lr
	
  /////////////////////////////////////////////////////////////////////////////
 
        .thumb_func
dummy_handler:  
        b .  // do nothing

	

	