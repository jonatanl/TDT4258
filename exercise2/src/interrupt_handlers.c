#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "sound.h"


// extern variable
struct playback_t test_playback;


/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler() 
{  
//  *(test_playback.dac_channel) = next_sample(&test_playback);
  *(test_playback.dac_channel) = next_sample(&test_playback);

  // Clear the interrupt 
  *TIMER1_IFC = 1;
}

/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) LETIMER0_IRQHandler() 
{  
//  *(test_playback.dac_channel) = next_sample(&test_playback);
  *(test_playback.dac_channel) = next_sample(&test_playback);

  // Clear the interrupt
  *LETIMER0_IFC |= (1 << 2);
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
