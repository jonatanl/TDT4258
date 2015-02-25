#include <stdint.h>

#include "sound.h"

//-----------------------------------------------------------------------------
// Implementation of a sound generator that creates samples for sounds composed
// of notes. For details, also check out sound.h.
//-----------------------------------------------------------------------------


// Sound variables
struct sound_t *current_sound;  // The current sound being played
uint32_t remaining_notes; // Remaining notes to play of the sound
uint32_t sampling_frequency; // Sampling frequency of the sound
uint32_t duration_unit; // Defines the length of the shortest note duration in milliseconds
uint32_t done = 1; // Indicates that the sound is done playing


// Note variables
struct note_t *current_note; // The current note being played
uint32_t current_sample; // The current sample to play from the current note
uint32_t remaining_samples; // Remaining note samples
uint32_t current_frequency; // Note frequency
uint32_t samples_per_period; // Samples per note waveform period


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
  return frequency_table[note->note] >> (11 - note->octave);
}


// Initialize the next note
void set_note(struct note_t *note)
{
  current_note = note;
  current_frequency = get_frequency(note);
  samples_per_period = (sampling_frequency / current_frequency);
  remaining_samples = (sampling_frequency * current_note->duration * duration_unit / 1000); 
}


// Initialize the next sound
void set_sound(uint32_t sf, uint32_t du, struct sound_t *sound)
{
  // Set sampling frequency and duration unit
  sampling_frequency = sf;
  duration_unit = du;

  // Initialize the sound
  current_sound = sound;
  current_sample = 0;
  remaining_notes = sound->num_notes;
  set_note(sound->first); // Assert: Sound is not empty
  done = 0;
}


// Return the next sample of the sound
uint32_t next_sample()
{
  // If there are no more samples, move on to the next note
  if(remaining_samples == 0){

    // Skip past notes until a non-empty note is found 
    do{
      current_note++;
      remaining_notes--;

    }while(remaining_notes > 0 && current_note->duration == 0);

    // If there are no more notes, the sound is done
    if(remaining_notes == 0){
      done = 1;
    }else{
      set_note(current_note);
    }
  }

  // If the sound is done, replay it
  if(done){
    set_sound(sampling_frequency, duration_unit, current_sound); 
    return next_sample();
  }

  // Compute and return the next sample of the current note
  uint32_t sample;
  if(current_sample % samples_per_period < (samples_per_period / 2)){
    sample = 1 << (current_note->amplitude + 5);
  }else{
    sample = 0;
  }
  current_sample++;
  remaining_samples--;
  return sample;
}
