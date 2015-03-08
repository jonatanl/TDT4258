#include <stdint.h>
#include <stdbool.h>
#include "efm32gg.h"


// function to setup and start the LETIMER
// might lose period variable
void setupLETIMER()
{
  //-----------------------------------
  // Set up the letimer
  //-----------------------------------

  // Start the Low Frequency Oscillator (LFXO) to run LETIMER
  *CMU_OSCENCMD |= (1 << 8);

  // Select LFXO to drive LFACLK
  *CMU_LFCLKSEL &= ~(0x3 << 0); // clear bits first
  *CMU_LFCLKSEL |= (2 << 0);
  *CMU_CTRL &= ~(3 << 18);	// Sets wake up time of LFXO clock for quick wakeup

  // Enable clock for the Low Energy Peripheral Interface
  *CMU_HFCORECLKEN0 |= (1 << 4);

  // Enable clock for LETIMER 
  *CMU_LFACLKEN0 |= (1 << 2);

  // Enable LETIMER0_COMP0 as top register for LETIMER
  *LETIMER0_CTRL |= (1 << 9);

  // Set COMP0 to zero to get one underflow interrupt per LFACLK cycle
  *LETIMER0_COMP0 = 0x0;

  // Start the LETIMER
  *LETIMER0_CMD |= (1 << 0);

  // Enable interrupts for LETIMER
  *LETIMER0_IEN |= (1 << 2);
}

void letimer_enable()
{
  // Start Low Frequency Oscillator (LFXO)
  *CMU_OSCENCMD |= (1 << 8);
}

void letimer_disable()
{
  // Stop Low Frequency Oscillator (LFXO)
  *CMU_OSCENCMD |= (1 << 9);
}


//-----------------------------------------
// Some example uses of the LETIMER module
//
// NOTE: Take care, this code is not tested
//-----------------------------------------
//
//Enable clock source for the LETIMER
//
//  // Enable clock for Low Energy Peripheral Interface
//  *CMU_HFCORECLKEN0 |= CMU_HFCORECLKEN0_LE;
//
//  // Enable clock for LETIMER 
//  *CMU_LFACLKEN0 |= CMU_LFACLKEN0_LETIMER0;
//
//
//Starting and stopping the LETIMER
//  *LETIMER0_CMD |= LETIMER0_CMD_START;
//  *LETIMER0_CMD |= LETIMER0_CMD_STOP;
//
//
//Clearing and reading the LETIMER value
//  *LETIMER0_CMD |= LETIMER0_CMD_CLEAR;
//  uint16_t value = *LETIMER0_CNT;
//
//
//Setting a top value less than 0xffff, and using buffered top value
//
//  // Enable top value
//  *LETIMER0_CTRL |= LETIMER0_CTRL_COMP0TOP;
//
//  // Set the top value. It is loaded into LETIMER0_CNT on underflow.
//  uint16_t top_value = 0x2fff;
//  *LETIMER0_COMP0 = top_value;
//
//  // Enable buffered top value. On underflow, LETIMER0_COMP1 is loaded into
//  // LETIMER0_COMP0.
//  uint16_t buffered_top_value = 0x4fff;
//  *LETIMER0_CTRL |= LETIMER0_CTRL_BUFTOP;
//  *LETIMER0_COMP0 = buffered_top_value;
//
//
//Prescale clock source for the LETIMER
//  uint32_t presc = 5;
//
//  // Clear the LETIMER prescale bits
//  *CMU_LFAPRESC0 = (*CMU_LFAPRESC0 & ~ (0xf << 8));
//
//  // Set the new LETIMER prescale bits
//  *CMU_LFAPRESC0 |= presc;
//
//
//Check if the LETIMER is running
//  bool running = *LETIMER0_STATUS;
//
//
//Features not documented here (but in the reference manual):
//
//  - Four different repeat modes can be set with the REPMODE bits of register
//    LETIMER0_CTRL.
//
//  - Four different underflow actions can be set for each of the two output
//    channels by setting bits UFOA0 and UFOA1 in LETIMER0_CTRL. This can be
//    used to create waveforms (square waves).
//
//  - The LETIMER can be triggered by RTC (Real-Time Clock).
//
//  - The LETIMER can trigger other peripherals through PRS
