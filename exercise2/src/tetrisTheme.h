#include <stdint.h> 
#include "synth.h"

uint16_t songArray[35] = {0x2447,0x1442,0x1443,0x2445,0x1443,0x1442,0x2440,0x1440,0x1443,0x2447,0x1445,0x1443,0x3442,0x1443,0x2445,0x2447,0x2443,0x2440,0x4440,0x1440,0x2445,0x1448,0x2440,0x144a,0x1448,0x3447,0x1443,0x2447,0x1445,0x1443,0x2442,0x1442,0x1443,0x2445,0x2447,};

//---------------------------------------------------------------------------
// This is an example of how songs should be created in order to make it easy
// to read them into the program.
//---------------------------------------------------------------------------
//
// number of notes for a part is 35;
// duration_unit is 60;
/*
uint16_t notes_part0[35] = { 1, 2, 3 };
synth_part part0 = { .start = (synth_note*)&notes_part0[0], .n_notes = 35, .channel = 0 };

uint16_t notes_part1[35] = { 1, 2, 3 };
synth_part part1 = { .start = (synth_note*)&notes_part1[0], .n_notes = 35, .channel = 1 };

synth_song tetrisSong = { .part0 = &part0, .part1 = &part1, .default_duration_unit = 60 };
*/
