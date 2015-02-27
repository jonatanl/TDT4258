#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

void setupDAC()
{
  // Setting up DAC per compendium instructions.

  // Enable DAC clock
  *CMU_HFPERCLKEN0 |= (1 << 17);

  // Prescale DAC clock with a factor of (2^7 + 1) = 128
  *DAC0_CTRL |= (7 << 16);

  // Enable DAC output to pin, but not ADC and ACMP
  *DAC0_CTRL |= 0x10;

  // Enable sine mode for DAC
  *DAC0_CTRL |= (1 << 1);

  //-----------
  // PRS stuff
  //-----------

  // Select PRS channel 0 as input channel
  *DAC0_CH0CTRL |= (0 << 4);

  // Set channel 0 to be triggered by PRS input
  *DAC0_CH0CTRL |= (1 << 2);

  // Enable left channel
  *DAC0_CH0CTRL |= 1;
}
