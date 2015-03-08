#include "efm32gg.h"
#include "letimer.h"
#include "dac.h"

// Go to EM3
void toggle_deep_sleep(){

  dac_disable();
  letimer_disable();

  disableDAC();
  disableLETIMER();

  *SCR |= 0x06;		// Sleep on exit, and deep sleep
  //*CMU_LFACLKEN0 = 0x00;
  //*CMU_LFBCLKEN0 = 0x00;
  //*CMU_LFCLKSEL = 0x00;
}

// Go to EM2
void toggle_sleep() 
{
  *SCR |= 0x06;		// Sleep on exit 
}

void silence(void){
    disableDAC();
    disableLETIMER();
    toggle_deep_sleep();
}

void start_sampler(){
    enable_sample_DAC();

    // Set sample mode
    *DAC0_CTRL |= 0x10;

    enableLETIMER();
}

void start_synth(){
    enable_synth_DAC();
    enableLETIMER();
}
