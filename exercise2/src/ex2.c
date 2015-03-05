#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

//-----------------------------------------------------------------------------
// This code initially demonstrated a possible defect in the efm32gg board
// labelled 119.  Now it shows the solution: LFRCO had variable frequency.
// Using LFXO, these problems disappeared.
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
#define CMU_LFCLKSEL     ((volatile uint32_t*)(CMU_BASE2 + 0x028))
#define CMU_LFACLKEN0    ((volatile uint32_t*)(CMU_BASE2 + 0x058))
#define CMU_LFAPRESC0    ((volatile uint32_t*)(CMU_BASE2 + 0x068))

// Interrupt bits in ISER0
#define IRQ_DAC			    (1 << 8)
#define IRQ_LETIMER0		(1 << 26)


int main(void) 
{  
  //-----------------------------------
  // Set up the letimer
  //-----------------------------------

//  // Start LFRCO that drives LFACLK
//  *CMU_OSCENCMD |= (1 << 6);
  // Start the Low Frequency Oscillator (LFXO)
  *CMU_OSCENCMD |= (1 << 8);

//  // Select LFRCO to drive LFACLK
//  *CMU_LFCLKSEL |= (1 << 0);
  // Select LFXO to drive LFACLK
  *CMU_LFCLKSEL &= ~(0x3 << 0);
  *CMU_LFCLKSEL |= (2 << 0);


  // Enable clock for LETIMER 
  *CMU_LFACLKEN0 |= (1 << 2);

  // Enable clock for the Low Energy Peripheral Interface
  *CMU_HFCORECLKEN0 |= (1 << 4);



  // Enable LETIMER0_COMP0 as top register for LETIMER
  *LETIMER0_CTRL |= (1 << 9);

  // Set COMP0 to zero to get one underflow interrupt per LFACLK cycle
  *LETIMER0_COMP0 = 0x0;

  // Set LFACLK clock prescaler for LETIMER
  *CMU_LFAPRESC0 |= (6 << 8);



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
  static int i=0;
  i = !i;
  if(i){
    *DAC0_CH0DATA = 128;
  }else{
    *DAC0_CH0DATA = 0;
  }

  // Clear the interrupt 
  *LETIMER0_IFC = (1 << 2);
}
