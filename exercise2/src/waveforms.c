#include <stdint.h>
#include "waveforms.h"

//----------------------------------------------------------------------------
// This code generates waveforms. Sample frequency and amplitude is configured
// through global variables, and note frequency and time is passed as
// parameters.
//
// The amplitude sets the maximum value of any sample.
//
// NOTE: Some waveforms are harder to hear than others, like saw and triangle.
//
// TODO: Generate sine waveform.
//----------------------------------------------------------------------------

uint32_t sample_frequency = 44100;
uint32_t amplitude = 256;

uint32_t waveform_square(uint32_t note_frequency, uint32_t time)
{
  uint32_t period = sample_frequency / note_frequency;
  uint32_t t = time % period;
  uint32_t sample;
  if(t < period/2){
    sample = amplitude;
  }else{
    sample = 0;
  }
  return sample;
}


uint32_t waveform_sawtooth(uint32_t note_frequency, uint32_t time)
{
  uint32_t period = sample_frequency / note_frequency;
  uint32_t t = time % period;
  uint32_t sample = (t * amplitude) / period;
  return sample;
}


uint32_t waveform_triangle(uint32_t note_frequency, uint32_t time)
{
  uint32_t period = sample_frequency / note_frequency;
  uint32_t t = time % period;
  uint32_t sample;
  if(t < period / 2){
    sample = (t * amplitude) / (period / 2);
  }else{
    sample = ((period - t) * amplitude) / (period / 2);
  }
  return sample;
}


// A polynomial waveform that simulates a sine wave. But actually, it sounds
// more like a triangle waveform.
uint32_t waveform_polynomial(uint32_t note_frequency, uint32_t time)
{
  uint32_t period = sample_frequency / note_frequency;
  uint32_t half_period = period / 2;
  uint32_t t = time % half_period;
  uint32_t sample;
//  sample = 3 * amplitude * half_period * t * t - 2 * amplitude * t * t * t;
//  sample /= half_period * half_period * half_period;
//  if(time % period > period / 2){
//    sample = amplitude - sample;
//  }
  sample = 3 * amplitude * half_period * t * t;
  sample -= 2 * amplitude * t * t * t;
  sample /= half_period * half_period * half_period;
  if(time % period > half_period){
    sample = amplitude - sample;
  }
  return sample;
}
