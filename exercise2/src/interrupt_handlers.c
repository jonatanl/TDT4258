#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler() 
{  
  /*
    TODO feed new samples to the DAC
    remember to clear the pending interrupt by writing 1 to TIMER1_IFC
  */

  
  *DAC0_CH0DATA += 128;

  if( *DAC0_CH0DATA >= 1050 ){
    *DAC0_CH0DATA -= 1024;  
  }
  
  

  
  // Calculate the next LEDs that display time
   int time = *GPIO_PA_DOUT >> 8;
  time = ~time;
  time = (time + 1) & 0xffff;
  time = ~time;
  *GPIO_PA_DOUT = time << 8;
  //

  // Clear the interrupt 
  *TIMER1_IFC = 1;
}

/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler() 
{
  // Blink the second LED from the left
  *GPIO_PA_DOUT ^= 0x0200;

  // clear the interrupt
  *GPIO_IFC = 0xff;
}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler() 
{
  // Blink the third LED from the left
  *GPIO_PA_DOUT ^= 0x0400;

  // clear the interrupt
  *GPIO_IFC = 0xff;
}
