#include "efm32gg.h"

// Go to EM3
void goToDeepSleep(){
  // Enable processor deep sleep
  *SCR |= 0x06; // Set this value to 0x06 to
  *CMU_LFACLKEN0 = 0x00;
  *CMU_LFBCLKEN0 = 0x00;
  *CMU_LFCLKSEL = 0x00;

  __asm("wfi");
}

// Go to EM2
void goToSleep() 
{
  *SCR |= 0x06;
  __asm("wfi");
}