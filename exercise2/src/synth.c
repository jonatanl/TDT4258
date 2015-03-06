#include <stdint.h>

#include "synth.h"
#include "efm32gg.h"

//-----------------------------------------------------------------------------
// Implementation of a sound generator that creates samples for sounds composed
// of notes. For details, also check out synth.h.
//-----------------------------------------------------------------------------


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
uint32_t get_frequency(synth_note *note)
{
  return frequency_table[note->pitch] >> (11 - note->octave);
}


// Returns the number of remaining samples for the specified note.
int get_remaining_samples(synth_note *note, synth_song_playback *song_playback)
{
  uint32_t duration = note->duration;
  uint32_t duration_unit = song_playback->duration_unit;
  uint32_t sampling_rate = song_playback->sampling_rate;
  return (duration * duration_unit * sampling_rate) / 1000;
}


// Resets the part playback, starting it over again.
void reset_part(synth_part_playback *part_playback)
{
  part_playback->remaining_notes = part_playback->part->n_notes - 1;
  part_playback->note = part_playback->part->start;
  part_playback->remaining_samples = get_remaining_samples(part_playback->note, part_playback->song_playback);
}


// Resets the part playback, starting it over again.
void restart_song(synth_song_playback *song_playback)
{
  reset_part(song_playback->part1_playback);
  reset_part(song_playback->part2_playback);
}


// Move the specified part playback on to the next note.
void next_note(synth_part_playback *part_playback)
{
  if(part_playback->remaining_notes > 0){
    part_playback->remaining_notes--;
    part_playback->note++;
    part_playback->remaining_samples = get_remaining_samples(part_playback->note, part_playback->song_playback);
  }else{

    // Reset the song when one part runs out of samples
    restart_song(part_playback->song_playback);
  } 
}


// Get next sample of the current playback
void synth_next_part_sample(synth_part_playback *part_playback)
{
  // Compute and return the next sample of the current note
  uint32_t sample;
  uint32_t remaining_samples = part_playback->remaining_samples;
  uint32_t sampling_rate = part_playback->song_playback->sampling_rate;
  uint32_t samples_per_period = sampling_rate / get_frequency(part_playback->note);
  if(remaining_samples % samples_per_period < samples_per_period / 2){
    sample = 1 << (part_playback->note->amplitude + 5);
  }else{
    sample = 0;
  }
  *(part_playback->dac_channel) = sample;

  // If there are no more samples, move on to the next note
  part_playback->remaining_samples--;
  if(part_playback->remaining_samples == 0){
    next_note(part_playback);
  }
}


// Play the next samples of the specified song playback.
void synth_next_song_sample(synth_song_playback *song_playback)
{
  synth_next_part_sample(song_playback->part1_playback);
  synth_next_part_sample(song_playback->part2_playback);
}


// Create a note in the memory specified by 'note'.
void synth_create_note(
    uint32_t pitch,
    uint32_t octave,
    uint32_t amplitude,
    uint32_t duration,
    synth_note *note /* output */ )
{
  // Stuff the note attributes into the note bitfield. Too large values are truncated using bitwise AND.
  note->pitch = pitch & 0xf;
  note->octave = octave & 0xf;
  note->amplitude = amplitude & 0x7;
  note->duration = duration & 0x1f;
}


// Create a part in the memory specified by 'part'.
void synth_create_part(
    synth_note *start,
    uint32_t n_notes,
    uint32_t channel,
    synth_part *part /* output */ )
{
  part->start = start;
  part->n_notes = n_notes;
  part->channel = channel;
}


// Create a song in the memory specified by 'song'.
void synth_create_song(
    synth_part *part1,
    synth_part *part2,
    uint32_t duration_unit,
    synth_song *song /* output */ )
{
  song->part1 = part1;
  song->part2 = part2;
  song->default_duration_unit = duration_unit;
}


// Create a part playback in the memory specified by 'part_playback'.
void synth_create_part_playback(
    synth_part *part,
    synth_song_playback *song_playback,
    synth_part_playback *part_playback /* output */ )
{
  part_playback->song_playback = song_playback;

  part_playback->part = part;
  reset_part(part_playback);

  // Set DAC channel where the part will be played
  switch(part->channel)
  {
    case 0: part_playback->dac_channel = DAC0_CH0DATA; break;
    case 1: part_playback->dac_channel = DAC0_CH1DATA; break;
    default:
      // ERROR: Code will only enter here on incorrect input.
      part_playback->dac_channel = 0;
      break;
  }

}

    
// Create a song playback in the memory specified by 'playback'.
void synth_create_song_playback(
    synth_song *song,
    synth_part_playback *part1_playback,
    synth_part_playback *part2_playback,
    uint32_t sampling_rate,
    synth_song_playback *song_playback /* output */ )
{
  song_playback->song = song;

  // Load pointers to allocated part playback memory
  song_playback->part1_playback = part1_playback;
  song_playback->part2_playback = part2_playback;

  // Load default playback parameters of the song
  song_playback->duration_unit = song->default_duration_unit;
  song_playback->sampling_rate = sampling_rate;

  // Create and load part playbacks
  synth_create_part_playback(song->part1, song_playback, song_playback->part1_playback);
  synth_create_part_playback(song->part2, song_playback, song_playback->part2_playback);
}
