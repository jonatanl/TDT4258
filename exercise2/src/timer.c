#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/* function to setup the timer */
void setupTimer(uint16_t period)
{
  // 1. Enable peripheral clock for timer 0 and timer 1
  *CMU_HFPERCLKEN0 |= CMU_HFPERCLKEN0_TIMER1;
  
  // 2. Write period for timer 0 and timer 1
  *TIMER1_TOP = period;

  // Extra: Set prescaling factor for peripheral clock to 2^10
  // (Timer now runs 1024 times slower)
//  *TIMER1_CTRL = (*TIMER1_CTRL) | (10 << 24);

  // 3. Enable interrupt generation
  *TIMER1_IEN = 1;

  // 4. Start the timer
  *TIMER1_CMD = 1;
}
