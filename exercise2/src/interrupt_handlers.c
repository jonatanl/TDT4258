#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

volatile uint32_t *channel_descriptor;
void setupDMA(volatile uint32_t *ch_desc);

/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler() 
{  
  // Blink the leftmost LED
  *GPIO_PA_DOUT ^= 0x0100;

  // Load channel descriptor
  setupDMA(channel_descriptor);

  //*GPIO_PA_DOUT = *(GPIO_PC_DIN) << 8;

  // clear the interrupt
  *TIMER1_IFC = 1;
}

/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler() 
{
  // Do nothing

  // clear the interrupt
  *GPIO_IFC = 0xff;
}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler() 
{
  // Do nothing

  // clear the interrupt
  *GPIO_IFC = 0xff;
}
