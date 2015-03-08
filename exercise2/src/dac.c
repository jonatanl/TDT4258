#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

// TODO: Heavy handed approach

void setupDAC()
{
  // Enable DAC clock
  *CMU_HFPERCLKEN0 |= (1 << 17);

  // Prescale DAC clock, currently 435.7KHz
  *DAC0_CTRL = 0x50000;

  // Set voltage source
  *DAC0_CTRL |= (2 << 8);

  // Set sample mode
  *DAC0_CTRL |= 0x10;

  // Enable left and right channel
  *DAC0_CH0CTRL = 1;
  *DAC0_CH1CTRL = 1;
}

void dac_enable(){
  *CMU_HFPERCLKEN0 |= (1 << 17);
}

void dac_disable(){
  *CMU_HFPERCLKEN0 &= ~(1 << 17);
}
