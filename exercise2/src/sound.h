
// Extern variable for testing purposes
extern struct sound_t test_sound;


//-----------------------------------------------------------------------------
// This struct defines several bitfields that are useful to pack a note into 16
// bits. This makes it possible to load several long sounds into the efm32gg.
//-----------------------------------------------------------------------------
struct note_t
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
	unsigned int note : 4;

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
};


// Set the next sound to be played
void set_sound(uint32_t, uint32_t, struct sound_t*);


// Get the next sample of the current sound played
uint32_t next_sample();
