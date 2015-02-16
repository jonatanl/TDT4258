#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/* function to setup the timer */
void setupTimer(uint16_t period)
{
  /*
    TODO enable and set up the timer
    
    1. Enable clock to timer by setting bit 6 in CMU_HFPERCLKEN0
    2. Write the period to register TIMER1_TOP
    3. Enable timer interrupt generation by writing 1 to TIMER1_IEN
    4. Start the timer by writing 1 to TIMER1_CMD
    
    This will cause a timer interrupt to be generated every (period) cycles. Remember to configure the NVIC as well, otherwise the interrupt handler will not be invoked.
  */  

  // 1. Enable peripheral clock for timer 0 and timer 1
  *CMU_HFPERCLKEN0 |= (1 << 6);
  
  // 2. Write period for timer 0 and timer 1
  *TIMER1_TOP = period;

  // Extra: Set prescaling factor for peripheral clock to 2^10
  // (Timer now runs 1024 times slower)
  *TIMER1_CTRL = (*TIMER1_CTRL) | (10 << 24);

  // 3. Enable interrupt generation
  *TIMER1_IEN = 1;

  // 4. Start the timer
  *TIMER1_CMD = 1;
}


