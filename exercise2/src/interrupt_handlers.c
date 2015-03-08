#include <stdint.h>
#include <stdbool.h>
#include "arraysound.h"
#include "efm32gg.h"
#include "dac.h"
#include "synth.h"

uint8_t button_mapper(void);

synth_song_playback test_playback;

void handle_gpio(void);

/* LETIMER0 interrupt handler */
void __attribute__ ((interrupt)) LETIMER0_IRQHandler() 
{  
  play();
  // Clear the interrupt
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
  
  // play_init(0);
  
  if(input == 0){return;}
    switch(input){
  case 1:
    play_init(0);	// btn 1
    break;
  case 2:		// btn 2
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
  case 8:		// btn 8
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





























