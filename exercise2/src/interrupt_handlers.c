#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "sound.h"

/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler() 
{  
  // Get the next sample for the current sound
  //*DAC0_CH0DATA = next_sample();

  // If DAC channel 0 has valid data
  if(*DAC0_STATUS & 1){
    *GPIO_PA_DOUT |= 0x0100;
    *GPIO_PA_DOUT &= (~ 0x0200) & 0xff00;
  }else{
    *GPIO_PA_DOUT |= 0x0200;
    *GPIO_PA_DOUT &= (~ 0x0100) & 0xff00;
  }

  // Clear the interrupt 
  *TIMER1_IFC = 1;
}

/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler() 
{
  // Blink the second LED from the left
  *GPIO_PA_DOUT ^= 0x8000;

  // clear the interrupt
  *GPIO_IFC = 0xff;
}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler() 
{
  // Blink the third LED from the left
  //*GPIO_PA_DOUT ^= 0x0400;

  // clear the interrupt
  *GPIO_IFC = 0xff;
}
