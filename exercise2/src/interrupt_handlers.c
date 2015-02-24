#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

//------------------------------------------------------------------------------
// The interrupt handlers in this file will read an address value defined by the
// linker script and output it to the gamepad LEDs. To make it possible to see
// all 32 bits of the address, gamepad buttons 1 and 3 can be used to shift the
// bits to the left and right.
//------------------------------------------------------------------------------


//--------------------------------------------------------
// Uncomment variables to change the address value of the
// symbol read from the linker script
//--------------------------------------------------------
uint32_t led_shift = 0;
//uint32_t led_shift2 = 5;
//uint32_t led_shift3 = 6;
//uint32_t led_shift4 = 7;
//uint32_t led_shift5 = 8;
//uint32_t led_shift6 = 9;
//uint32_t led_shift7 = 10;
//uint32_t led_shift8 = 11;


/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler() 
{  
  // Do nothing
  
  // clear the interrupt
  *TIMER1_IFC = 1;
}

// Common GPIO handler for even and odd interrupts
void GPIO_BOTH_IRQHandler()
{

  // Calculate the number of the button that were pushed by
  // reading the interrupt from GPIO_IF
  int button_num = 0;
  for(int i=*GPIO_IF; i>0; i/=2){
	button_num++;
  }

  // Update the led_shift value depending on which button
  // was pushed:
  //  - button 1: Increase shift
  //  - button 3: Decrease shift
  if(button_num == 1 && led_shift < 31){
	  led_shift++;
  }else if(button_num == 3 && led_shift > 0){
	  led_shift--;
  }

  // Read the address value of the symbol __data_end__ defined by the linker
  // script. This value is right-shifted a number of times equal to the value of
  // 'led_shift'. Then the 8 lower-most bits is written to the gamepad LEDs.
  extern char __data_end__[];
  *GPIO_PA_DOUT = ((((uintptr_t)__data_end__) >> led_shift) << 8) & 0xff00;

  // clear the interrupt
  *GPIO_IFC = 0xff;
}

/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler() 
{
	GPIO_BOTH_IRQHandler();
}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler() 
{
	GPIO_BOTH_IRQHandler();
}
