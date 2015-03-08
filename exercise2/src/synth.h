#include <stdint.h>

// Include guard for synth.h avoids redefinition of structs.
#ifndef SYNTH_H
#define SYNTH_H

// typedefs to avoid having "struct foo" all around.
typedef struct synth_note synth_note;
typedef struct synth_part synth_part;
typedef struct synth_song synth_song;
typedef struct synth_song_playback synth_song_playback;
typedef struct synth_part_playback synth_part_playback;


// Macro definitions for testing purposes
#define A   0
#define Ax  1
#define H   2
#define C   3
#define Cx  4
#define D   5
#define Dx  6
#define E   7
#define F   8
#define Fx  9
#define G   10
#define Gx  11


//-----------------------------------------------------------------------------
// This struct defines several bitfields that are useful to pack a note into 16
// bits. This makes it possible to load several long sounds into the efm32gg.
//-----------------------------------------------------------------------------
struct __attribute__ ((__packed__)) synth_note
{
  // The note pitch. 
  //   0 = A
  //   1 = A#
  //   2 = H
  //   3 = C
  //   4 = C#
  //   5 = D
  //   6 = D#
  //   7 = E
  //   8 = F
  //   9 = F#
  //  10 = G
  //  11 = G#
  // 
  // NOTE: Numbers above 11 are not supported.
  unsigned int pitch : 4;

  // The note octave.
  //
  // Using pitch and octave, the lowest note possible is A0 at 27.5 Hz, and the
  // highest is A10 at 28160 Hz.
  // 
  // NOTE: Numbers above 10 are not supported.
  unsigned int octave : 4;

  // The note amplitude.
  //
  // The actual amplitude is calculated from this bitfield as 2^(amplitude + 5).
  unsigned int amplitude : 3;

  // The note duration.
  //
  // Five bits allow for 32 individual durations in an linear arrangement. The
  // actual duration of a note will depend on what duration unit is set in the
  // song or the playback structs.
  unsigned int duration : 5;
};


// A part has a list of notes.
struct synth_part
{
  synth_note *start;
  uint32_t n_notes;
  uint32_t channel;
};


// A song has two parts.
struct synth_song
{
  synth_part* part0;
  synth_part* part1;
  uint32_t default_duration_unit;
};


// A "part playback" represents the playback of a single part.
struct synth_part_playback
{
  // The song playback using this part
  synth_song_playback *song_playback;

  synth_part *part;               // The part being played
  uint32_t remaining_notes;       // Remaining notes to play
  volatile uint32_t *dac_channel; // Output DAC channel

  synth_note *note;           // The current note being played
  uint32_t remaining_samples; // Remaining samples to be fed to the DAC
};


// A "song playback" represents the playback of a single song using two part playbacks.
struct synth_song_playback
{
  synth_song *song; // The song being played

  // The current duration unit and sample rate of the song.
  uint32_t duration_unit;
  uint32_t sampling_rate;

  // The two part playbacks of this song.
  synth_part_playback *part0_playback;
  synth_part_playback *part1_playback;
};


// Create a note in the memory specified by "*note".
void synth_create_note(
    uint32_t pitch,
    uint32_t octave,
    uint32_t amplitude,
    uint32_t duration,
    synth_note *note /* output */ );


// Create a part in the memory specified by "*part".
void synth_create_part(
    synth_note *start,
    uint32_t n_notes,
    uint32_t channel,
    synth_part *part /* output */ );


// Create a song in the memory specified by 'song'.
void synth_create_song(
    synth_part *part0,
    synth_part *part1,
    uint32_t duration_unit,
    synth_song *song /* output */ );


// Create a part playback in the memory specified by 'part_playback'.
void synth_create_part_playback(
    synth_part *part,
    synth_song_playback *song_playback,
    synth_part_playback *part_playback /* output */ );


// Create a playback in the memory specified by "*playback".
void synth_create_song_playback(
    synth_song *song,
    synth_part_playback *part0_playback,
    synth_part_playback *part1_playback,
    uint32_t sampling_rate,
    synth_song_playback *song_playback  /* output */ );


// Play the next samples of the specified song playback.
void synth_next_song_sample(synth_song_playback *song_playback);


#endif /* SYNTH_H */
