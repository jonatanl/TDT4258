#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

void setupDAC()
{
  // Setting up DAC per compendium instructions.

  // Enable DAC clock
  *CMU_HFPERCLKEN0 |= (1 << 17);

  // Prescale DAC clock, currently 435.7KHz
  *DAC0_CTRL = 0x50000;

  // Set continuous mode
  *DAC0_CTRL |= 0x10;

  // Enable left and right channel
  *DAC0_CH0CTRL = 1;
  *DAC0_CH1CTRL = 1;
}
