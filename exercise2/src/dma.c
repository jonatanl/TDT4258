#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "efm32gg.h"

uint32_t value = 0x5500;

/* function to setup the DMA controller */
void setupDMA(volatile uint32_t *ch_desc)
{
  // Set channel 0 to be triggered on underflow/overflow in TIMER1
  *DMA_CH0_CTRL = (0x19 << 16);

  // Set src_data_end_ptr to read from GPIO buttons
  ch_desc[0] = ((uintptr_t)GPIO_PC_DIN); 

  // Set dst_data_end_ptr to write to GPIO LEDs
  ch_desc[1] = ((uintptr_t)GPIO_PA_DOUT) + 0x1;

  // Set the channel_cfg
  ch_desc[2] = 0;
  ch_desc[2] |= (0x2 << 0);   // cycle_ctrl = basic
  ch_desc[2] |= (0 << 4);     // n_minus_1 = 0 (do a single DMA transfer only)
  ch_desc[2] |= (10 << 14);   // R_power = 10
  // I'm betting the *_prot_ctrl stuff doesn't apply to me
  ch_desc[2] |= (0 << 24);    // src_size = 1 byte
  ch_desc[2] |= (0 << 26);    // src_inc = 1 byte
  ch_desc[2] |= (0 << 28);    // dst_size = 1 byte
  ch_desc[2] |= (0 << 30);    // dst_inc = 1 byte

  ch_desc[3] = 0;

  // Load address of channel descriptor table into DMA
  *DMA_CTRLBASE = ch_desc;

  // Enable the DMA
  *DMA_CONFIG |= 1;

  // Use the primary structure for channel 0
  *DMA_CHALTC |= (0x1 << 0);

  // Enable channel 0
  *DMA_CHENS  |= (0x1 << 0);

  *GPIO_PA_DOUT ^= 0x0200;
}
