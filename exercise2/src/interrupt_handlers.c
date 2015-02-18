#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler() 
{  
  // Blink the leftmost LED
  *GPIO_PA_DOUT ^= 0x0100;

  // clear the interrupt
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
