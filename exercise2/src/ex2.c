#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

//-----------------------------------------------------------------------------
// This code demonstrates a possible defect in the efm32gg board labelled 119:
//
// When the module LETIMER0 is used to drive interrupts, and the MCU uses those
// interrupts to drive an audible square waveform to the DAC, a slight
// variation in pitch can be heard.
//
// This code was simplified as much as possible to avoid bugs. It based on the
// exercise code framework, except it uses a single file for all code.
//-----------------------------------------------------------------------------


//-----------------------------------
// Additional register definitions
//-----------------------------------

// LETIMER registers
#define LETIMER0_BASE   0x40082000 
#define LETIMER0_CTRL   ((volatile uint32_t*)(LETIMER0_BASE + 0x00))
#define LETIMER0_CMD    ((volatile uint32_t*)(LETIMER0_BASE + 0x04))
#define LETIMER0_COMP0  ((volatile uint32_t*)(LETIMER0_BASE + 0x10))
#define LETIMER0_IFC    ((volatile uint32_t*)(LETIMER0_BASE + 0x28))
#define LETIMER0_IEN    ((volatile uint32_t*)(LETIMER0_BASE + 0x2c))

// CMU registers used to start LETIMER clock
#define CMU_OSCENCMD     ((volatile uint32_t*)(CMU_BASE2 + 0x020))
#define CMU_LFACLKEN0    ((volatile uint32_t*)(CMU_BASE2 + 0x058))

// Interrupt bits in ISER0
#define IRQ_DAC			    (1 << 8)
#define IRQ_LETIMER0		(1 << 26)


int main(void) 
{  
  //-----------------------------------
  // Set up the letimer
  //-----------------------------------

  // Start LFRCO that drives LFACLK
  *CMU_OSCENCMD |= (1 << 6);

  // Enable clock for the Low Energy Peripheral Interface
  *CMU_HFCORECLKEN0 |= (1 << 4);

  // Enable clock for LETIMER 
  *CMU_LFACLKEN0 |= (1 << 2);

  // Enable LETIMER0_COMP0 as top register for LETIMER
  *LETIMER0_CTRL |= (1 << 9);

  // Set COMP0 to zero to get one underflow interrupt per LFACLK cycle
  *LETIMER0_COMP0 = 0;

  // Enable interrupts for LETIMER
  *LETIMER0_IEN |= (1 << 2);

  // Start the LETIMER
  *LETIMER0_CMD |= (1 << 0);


  //-----------------------------------
  // Set up the DAC
  //-----------------------------------

  // Enable DAC clock
  *CMU_HFPERCLKEN0 |= (1 << 17);

  // Enable DAC output to pin, but not ADC and ACMP
  *DAC0_CTRL |= 0x10;

  // Enable left DAC channel
  *DAC0_CH0CTRL |= 1;

 
  //-----------------------------------
  // Set up the NVIC
  //-----------------------------------

  // Enable DAC interrupt generation
  *ISER0 |= IRQ_DAC;
  
  // Enable letimer interrupt generation
  *ISER0 |= IRQ_LETIMER0;


  //-----------------------------------
  // Loop and wait for interrupts
  //-----------------------------------
  while(1);

  return 0;
}


void __attribute__ ((interrupt)) LETIMER0_IRQHandler() 
{  
  // Generate a note with a frequency of
  //    32768 / 64 = 512
  // 
  // and an amplitude of 128.
  //
  // This tone should be pure, without any variation in
  // pitch. However, this is not the case on all boards.
  static int i=0;
  i = (i+1) % 64;
  if(i<32){
    *DAC0_CH0DATA = 128;
  }else{
    *DAC0_CH0DATA = 0;
  }

  // Clear the interrupt 
  *LETIMER0_IFC = (1 << 2);
}
