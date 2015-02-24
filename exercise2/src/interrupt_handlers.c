#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "waveforms.h"

//----------------------------------------------------------------------
// Uses waveforms.c to generate waveforms. TODO: Use different waveforms
// depending on buttons.
//----------------------------------------------------------------------

uint32_t time = 0;

/* TIMER1 interrupt handler */
void __attribute__ ((interrupt)) TIMER1_IRQHandler() 
{  
  // Feed the DAC with a waveform generated by waveform.c
//  *DAC0_CH0DATA = waveform_square(220, time);
//  *DAC0_CH0DATA = waveform_sawtooth(220, time);
  *DAC0_CH0DATA = waveform_triangle(220, time);
  
  // Always increase timer
  time++;
  
  // Clear the interrupt 
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
