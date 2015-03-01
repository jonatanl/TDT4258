#include <stdint.h>

#include "sound.h"
#include "efm32gg.h"

//-----------------------------------------------------------------------------
// Implementation of a sound generator that creates samples for sounds composed
// of notes. For details, also check out sound.h.
//-----------------------------------------------------------------------------


//// Sound variables
//struct sound_t *current_sound;  // The current sound being played
//uint32_t remaining_notes; // Remaining notes to play of the sound
//uint32_t sampling_frequency; // Sampling frequency of the sound
//uint32_t duration_unit; // Defines the length of the shortest note duration in milliseconds
//uint32_t done = 1; // Indicates that the sound is done playing
//
//
//// Note variables
//struct note_t *current_note; // The current note being played
//uint32_t current_sample; // The current sample to play from the current note
//uint32_t remaining_samples; // Remaining note samples
//uint32_t current_frequency; // Note frequency
//uint32_t samples_per_period; // Samples per note waveform period


// Table of accurate frequency samples for converting notes
// to frequencies.
uint16_t frequency_table[13] = {
    28160,  // A10
    29834,  // A#
    31609,  // H
    33488,  // C
    35479,  // C#
    37589,  // D
    39824,  // D#
    42192,  // E
    44701,  // F
    47359,  // F#
    50175,  // G
    53159,  // G#
};


// Returns the frequency of the specified note
uint32_t get_frequency(struct note_t *note)
{
  return frequency_table[note->pitch] >> (11 - note->octave);
}


// Create a note in the memory specified by "*note".
void create_note(
    struct note_t *note,  // output
    uint32_t pitch,
    uint32_t octave,
    uint32_t amplitude,
    uint32_t duration)
{
  // Stuff the note attributes into the note bitfield. Too large values are truncated using bitwise AND.
  note->pitch = pitch & 0xf;
  note->octave = octave & 0xf;
  note->amplitude = amplitude & 0x7;
  note->duration = duration & 0x1f;
}


// Create a sound in the memory specified by "*sound".
void create_sound(
    struct sound_t *sound,  // output
    struct note_t *first,
    uint32_t num_notes)
{
  sound->num_notes = num_notes;
  sound->first = first;
  sound->last = first + num_notes - 1;
}


// Create a playback in the memory specified by "*playback".
void create_playback(
    struct playback_t *playback,  // output
    struct sound_t *sound,
    uint32_t sampling_frequency,
    uint32_t duration_unit,
    volatile uint32_t *dac_channel)
{
  playback->sampling_frequency = sampling_frequency;
  playback->duration_unit = duration_unit;
  playback->dac_channel = dac_channel;
  playback->current_sound = sound;

  // Initially, the first note points to the note BEFORE the first note
  playback->current_note = sound->first - 1;

  // Initialize to zero
  playback->note_frequency = 0;
  playback->samples_per_period = 0;
  playback->remaining_samples = 0;

  next_note(playback);
}


// Move the specified playback on to the next note
void next_note(struct playback_t * playback)
{
  // Increment note pointer "current_note".
  //
  // TODO: Notes with duration zero may cause problems.
  if(playback->current_note < playback->current_sound->last){
    playback->current_note += 1;
  }else{
    playback->current_note = playback->current_sound->first; // Play the sound in a loop
  }

  // Update variables relating to the current note
  playback->note_frequency = get_frequency(playback->current_note);
  playback->samples_per_period = playback->sampling_frequency / playback->note_frequency;
  playback->remaining_samples = (playback->sampling_frequency * playback->duration_unit) / 1000;
}


// Get next sample of the current playback
uint32_t next_sample(struct playback_t *playback)
{
  // If there are no more samples, move on to the next note
  if((int32_t)(playback->remaining_samples) <= 0){
    *GPIO_PA_DOUT ^= 0xff00;
    next_note(playback);
  }
  *GPIO_PA_DOUT = playback->remaining_samples;

  // Compute and return the next sample of the current note
  uint32_t sample;
  if(playback->remaining_samples % playback->samples_per_period < (playback->samples_per_period / 2)){
    sample = 1 << (playback->current_note->amplitude + 5);
  }else{
    sample = 0;
  }
  playback->remaining_samples--;
  return sample;
}
