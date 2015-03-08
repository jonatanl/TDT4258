#include "efm32gg.h"
#include "letimer.h"
#include "dac.h"

// Go to EM2
void toggle_sleep() 
{
  *SCR |= 0x06;		// Sleep on exit 

  //*CMU_LFACLKEN0 = 0x00;
  //*CMU_LFBCLKEN0 = 0x00;
  //*CMU_LFCLKSEL = 0x00;
}

void silence(void){
    dac_disable();
    letimer_disable();
    toggle_sleep();
}

void start_sampler(){

    // Set sample mode for DAC
    *DAC0_CTRL |= 0x11;

    dac_enable();
    letimer_enable();
}

void start_synth(){

    // Set sample mode for DAC
    *DAC0_CTRL &= ~(0x11);
    *DAC0_CTRL |= 0x10;

    dac_enable();
    letimer_enable();
}
