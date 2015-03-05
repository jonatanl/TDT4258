
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

// Extern variable for testing purposes
extern struct playback_t playback;

//-----------------------------------------------------------------------------
// This struct defines several bitfields that are useful to pack a note into 16
// bits. This makes it possible to load several long sounds into the efm32gg.
//-----------------------------------------------------------------------------
struct __attribute__ ((__packed__)) note_t
{
    // Bitfield used to designate pitch of a note:
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

	// Bitfield used to designate the octave of the note. The lowest note
	// possible is A0 at 27.5 Hz. The highest note is A10 at 28160 Hz.
	// 
    // NOTE: Numbers above 10 are not supported.
	unsigned int octave : 4;

	// Bitfield used to designate amplitude of the note. The actual amplitude
	// is calculated from this bitfield as 2^(amplitude + 5).
	unsigned int amplitude : 3;

    // Duration of a note. Five bits allow for 32 individual durations. Note
    // that the actual duration of a note will depend on what duration unit is
    // used in the synthesizer.
	unsigned int duration : 5;
};


// A sound has a list of notes
struct sound_t
{
	uint32_t num_notes;
	struct note_t *first;
	struct note_t *last;
};


// A sound that is being played
struct playback_t
{
  // Variables related to the playback itself
  uint32_t sampling_frequency;
  uint32_t duration_unit;
  volatile uint32_t *dac_channel;

  // Variables related to the current note being played
  struct sound_t *current_sound;
  struct note_t *current_note;
  uint32_t note_frequency;
  uint32_t samples_per_period;
  uint32_t remaining_samples;
};


// Create a note in the memory specified by "*note".
void create_note(
    struct note_t *note,  // output
    uint32_t pitch,
    uint32_t octave,
    uint32_t amplitude,
    uint32_t duration);


// Create a sound in the memory specified by "*sound".
void create_sound(
    struct sound_t *sound,  // output
    struct note_t *first,
    uint32_t num_notes);


// Create a playback in the memory specified by "*playback".
void create_playback(
    struct playback_t *playback,  // output
    struct sound_t *sound,
    uint32_t sampling_frequency,
    uint32_t duration_unit,
    volatile uint32_t *dac_channel);


// Move the specified playback on to the next note
void next_note(struct playback_t * playback);


// Get next sample of the current playback
uint32_t next_sample(struct playback_t *playback);
