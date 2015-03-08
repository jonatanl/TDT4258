#include <stdint.h>
#include <stdbool.h>
#include "arraysound.h"
#include "efm32gg.h"
#include "dac.h"
#include "synth.h"

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
 
 // Sanitize input
  static int last_input;
  int input= *GPIO_PC_DIN;
  int pressed = input ^ last_input;
  pressed = pressed & input;
  
  *GPIO_PA_DOUT = pressed;
  play_init(0);

  // Map to effects
  switch(pressed){
  case ~0b1:
    // btn 0
    play_init(0);
    break;
  case ~0b10:
    // btn 1
    play_init(1);
    break;
  case ~0b100:
    // btn 2
    play_init(2);
    break;
  case ~0b1000:
    // btn 3
    play_init(3);
    break;
  case ~0b10000:
    // btn 4
    break;
  case ~0b100000:
    // btn 5
    break;
  case ~0b1000000:
    // btn 6
    break;
  case ~0b10000000:
    // btn 7
  default:
    break;
  }
}

