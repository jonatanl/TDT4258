#include <stdint.h>
#include <stdbool.h>
#include "arraysound.h"
#include "efm32gg.h"
#include "dac.h"
#include "synth.h"
#include "sleepControl.h"
#include "letimer.h"

uint8_t button_mapper(void);
void silence(void);

synth_song_playback test_playback;

// 0 - array, 1 - synth 
int mode = -1;

void handle_gpio(void);

/* LETIMER0 interrupt handler */
void __attribute__ ((interrupt)) LETIMER0_IRQHandler() 
{

    switch(mode){
  case 0:
    play();
    break;
  case 1:    
    synth_next_song_sample(&test_playback);
    break;
  default:
    break;
  }

  (*GPIO_PA_DOUT)++;
  *LETIMER0_IFC |= (1 << 2);
}

/* GPIO even pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_EVEN_IRQHandler() 
{
  handle_gpio();
  // clear the interrupt
  *GPIO_IFC = 0xff;
}

/* GPIO odd pin interrupt handler */
void __attribute__ ((interrupt)) GPIO_ODD_IRQHandler() 
{
  handle_gpio();
  // clear the interrupt
  *GPIO_IFC = 0xff;
}


/* Maps a buttonpress to a song or effect  */
void handle_gpio(void){
int input = button_mapper();
  
  if(input == 0){return;}
    switch(input){
  case 1:
    mode = 0;
    play_init(0);
    start_sampler();
    break;
  case 2:
    mode = 1;
    start_synth();
    break;
  case 3:		// btn 3
    break;
  case 4:		// btn 4
    break;
  case 5:		// btn 5
    break;
  case 6:		// btn 6
    break;
  case 7:		// btn 7
    break;
  case 8:
    silence();		// btn 8
    break;
  default:
    break;
  }
  return;
}



uint8_t button_mapper(void){
  uint8_t input= ~(*GPIO_PC_DIN);
  *GPIO_PA_DOUT = input << 8;
    switch(input){
  case 0x1:
    return 1;
  case 0x2:
    return 2;
  case 0x4:
    return 3;
  case 0x8:
    return 4;
  case 0x10:
    return 5;
  case 0x20:
    return 6;
  case 0x40:
    return 7;
  case 0x80:
    return 8;
  default:
    break;
  }
  return 0;
}





























