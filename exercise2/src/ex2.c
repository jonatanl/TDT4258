#include <stdint.h>
#include <stdbool.h>
#include "efm32gg.h"
#include "synth.h"
#include "dac.h"
#include "letimer.h"
#include "sleepControl.h"

#define   SAMPLING_RATE   32768

// Declaration of peripheral setup functions
void setupNVIC();
void setupGPIO();

synth_song tetrisSong;
synth_song marioSong;

synth_song_playback tetrisPlayback;
synth_song_playback marioPlayback;

synth_part_playback tetris_part1;
synth_part_playback tetris_part2;
synth_part_playback mario_part1;
synth_part_playback mario_part2;

int main(void) 
{  
  synth_create_song_playback(&tetrisSong, &tetris_part1, &tetris_part2, SAMPLING_RATE, &tetrisPlayback); 
  synth_create_song_playback(&marioSong, &mario_part1, &mario_part2, SAMPLING_RATE, &marioPlayback); 
  
  setupGPIO();
  setupDAC();
  setupLETIMER();
  setupNVIC();

  silence();
  return 0;
}

void setupNVIC()
{
  //------------------------------------------------------
  // Enable interrupt handling for the various components 
  //------------------------------------------------------

  // Enable TIMER1 interrupt generation
  *ISER0 |= IRQ_TIMER1;

  // Enable DMA interrupt generation
  *ISER0 |= IRQ_DMA;

  // Enable odd and even GPIO interrupt generation
  *ISER0 |= (IRQ_GPIO_EVEN | IRQ_GPIO_ODD);

  // Enable DAC interrupt generation
  *ISER0 |= IRQ_DAC;

  // Enable LEUART0 interrupt generation
  *ISER0 |= IRQ_LEUART0;

  // Enable LETIMER0 interrupt 
  *ISER0 |= IRQ_LETIMER0;
}

/* if other interrupt handlers are needed, use the following names: 
   NMI_Handler
   HardFault_Handler
   MemManage_Handler
   BusFault_Handler
   UsageFault_Handler
   Reserved7_Handler
   Reserved8_Handler
   Reserved9_Handler
   Reserved10_Handler
   SVC_Handler
   DebugMon_Handler
   Reserved13_Handler
   PendSV_Handler
   SysTick_Handler
   DMA_IRQHandler
   GPIO_EVEN_IRQHandler
   TIMER0_IRQHandler
   USART0_RX_IRQHandler
   USART0_TX_IRQHandler
   USB_IRQHandler
   ACMP0_IRQHandler
   ADC0_IRQHandler
   DAC0_IRQHandler
   I2C0_IRQHandler
   I2C1_IRQHandler
   GPIO_ODD_IRQHandler
   TIMER1_IRQHandler
   TIMER2_IRQHandler
   TIMER3_IRQHandler
   USART1_RX_IRQHandler
   USART1_TX_IRQHandler
   LESENSE_IRQHandler
   USART2_RX_IRQHandler
   USART2_TX_IRQHandler
   UART0_RX_IRQHandler
   UART0_TX_IRQHandler
   UART1_RX_IRQHandler
   UART1_TX_IRQHandler
   LEUART0_IRQHandler
   LEUART1_IRQHandler
   LETIMER0_IRQHandler
   PCNT0_IRQHandler
   PCNT1_IRQHandler
   PCNT2_IRQHandler
   RTC_IRQHandler
   BURTC_IRQHandler
   CMU_IRQHandler
   VCMP_IRQHandler
   LCD_IRQHandler
   MSC_IRQHandler
   AES_IRQHandler
   EBI_IRQHandler
   EMU_IRQHandler
*/
