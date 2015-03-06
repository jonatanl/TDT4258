#include <stdint.h>
#include <stdbool.h>

#include "efm32gg.h"
#include "synth.h"
#include "tetrisTheme.h"


/* 
  TODO calculate the appropriate sample period for the sound wave(s) 
  you want to generate. The core clock (which the timer clock is derived
  from) runs at 14 MHz by default. Also remember that the timer counter
  registers are 16 bits.
*/
/* The period between sound samples, in clock cycles */
#define   SAMPLE_PERIOD   333 // Not used
#define   SAMPLING_RATE   32768

/* Declaration of peripheral setup functions */
void setupTimer(uint32_t period);
void setupDAC();
void setupNVIC();
void setupGPIO();
void setupLETIMER(uint16_t period);

// extern variables
synth_song_playback test_playback;

/* Your code will start executing here */
int main(void) 
{  
  /* Call the peripheral setup functions */
  setupGPIO();
  setupDAC();
//  setupTimer(SAMPLE_PERIOD);
  setupLETIMER(SAMPLE_PERIOD);


  //-------------------------
  // Creating a random sound
  //-------------------------

  // Create notes for channel 0
  synth_note notes1[12];
  for(int i=0; i<=12; i += 4){
    int pitch  = i % 12;
    int octave = 4 + (i / 12);
    int amplitude = 3;
    int duration = 1;
    synth_create_note(pitch, octave, amplitude, duration, &notes1[i]);
  }

  // Create part for channel 0
  synth_part part1;
  synth_create_part(&notes1[0], 12, 0 /* channel */, &part1);

  // Create notes for channel 1
  synth_note notes2[12];
  for(int i=12; i>=0; i -= 4){
    int pitch  = i % 12;
    int octave = 4 + (i / 12);
    int amplitude = 3;
    int duration = 1;
    synth_create_note(pitch, octave, amplitude, duration, &notes1[i]);
  }

  // Create part for channel 1
  synth_part part2;
  synth_create_part(&notes2[0], 12, 1 /* channel */, &part2);

  // Create a song
  synth_song song;
  synth_create_song(&part1, &part2, 1000, &song); 

  // Create a song playback in extern variable
  synth_create_song_playback(&song, SAMPLING_RATE, &test_playback);


//  synth_part test_part;
//  const int n_notes = 35;
//
//  synth_note notes[n_notes];
//  for(int i=0; i<12; i++){
//    notes[i].pitch = i % 12;
//    notes[i].octave = 4 + ((i == 12) ? 1 : 0);
//    notes[i].amplitude = (i / 2);
//    notes[i].duration = 1;
//  }
  // Creating a random sound
//  const int n_notes = 35;
//
//  synth_song song;

//  synth_part test_part;
//
//  //create_part(&test_part, (synth_note*)&songArray, n_notes);
//  create_part(&test_part, (synth_note*)&songArray, 35);
//
//  // Put the part into a playback
//  create_song_playback(&test_playback, &test_part, 44100, 60, DAC0_CH0DATA);

  /* Enable interrupt handling */
  setupNVIC();
  
  /* TODO for higher energy efficiency, sleep while waiting for interrupts
     instead of infinite loop for busy-waiting
  */
  while(1);

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
