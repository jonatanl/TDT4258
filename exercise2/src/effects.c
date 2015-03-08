#include "efm32gg.h"

uint32_t effects_sampling_rate = 32768;

void effects_play1()
{
  static uint32_t period = 0;
  static uint32_t remaining_samples = 0;

  // Decrease remaining samples
  if(remaining_samples == 0){

    // Update period
    if(period == 20){
      period = 0;
    }
    period++;
    remaining_samples = (period * effects_sampling_rate) / 1000;
  }
  remaining_samples--;

  // Feed data to DAC
  if(remaining_samples % period < period / 2){
    *DAC0_CH0DATA = 200; 
    *DAC0_CH1DATA = 200; 
  }else{
    *DAC0_CH0DATA = 0; 
    *DAC0_CH1DATA = 0; 
  }
}

void effects_play2()
{
  static uint32_t remaining_samples = 0;
  static int period = 0;

  if(remaining_samples == 0){

    period++;
    if(period > 10){
      period = -period;
    }
    remaining_samples = 1 + (period * period * effects_sampling_rate) / 1000;
  }
  remaining_samples--;

  // Feed data to DAC
  if(remaining_samples % (period * period) < (period * period) / 2){
    *DAC0_CH0DATA = 200; 
    *DAC0_CH1DATA = 200; 
  }else{
    *DAC0_CH0DATA = 0; 
    *DAC0_CH1DATA = 0; 
  }
}
