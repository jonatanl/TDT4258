#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"

/* function to set up GPIO mode and interrupts*/
void setupGPIO()
{

  // Enable GPIO clock  
  *CMU_HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;
  
  // Set high drive strength
  *GPIO_PA_CTRL = 2;
  
  // Set pins 8-15 to output
  *GPIO_PA_MODEH = 0x55555555;

  // Set all lights to 0, turning them off
  *GPIO_PA_DOUT = 0x0000;



  // Set pins 0-7 to input
  *GPIO_PC_MODEL = 0x33333333;

  // Enable pull-up
  *GPIO_PC_DOUT = 0xff;

  // Enable button interrupts
  *GPIO_EXTIPSELL = 0x22222222;

  // Set interrupt on 1->0
  *GPIO_EXTIFALL = 0xff;
  
  // Set interrupt on 0->1
  *GPIO_EXTIRISE = 0xff;

  // Enable interrupt generation
  *GPIO_IEN = 0xff;
}
