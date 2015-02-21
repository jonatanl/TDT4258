// This might not be the worlds most perfect code, but it's not that hard to debug, and efficiency isnt really an issue

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <stdbool.h>

#define PI 3.1415
#define Euler 2.7182

int RATE = 44100;
int MAX = 4095;

unsigned short normalize(float sine, int MAX);
float get_frequency(char* note);
void generate_song(char* note);
unsigned short* fill_sample_array(int tones, float* frequencies, int vibrato, int length);
float intensity(int sample);
float calculate_frequency_factor(int current, int length, float* frequencies);

// Creates an array of values between 0 and MAX. 
void main(int argc, char** argv){
	time_t t;
	srand((unsigned) time(&t));

	// This just needs to somewhat work, ugly code ahead
	// TODO make pretty
	int input = 1;
	
	char* name[16];
	int in = scanf("%20s", name);

	fprintf(stderr, "scanning %s\n", name);

	while(input != EOF){
		char note [50];
		input = scanf("%50s", note);
		if(input == EOF){
			break;
		}
		generate_sample(note);
	}
	return;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////
//////////////////////		HELPERS
//////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////



// Takes in a string and decodes the Note. 
// Typical input:
// 3E#4, vv3A4~A5
// NYI: vibrato should be handled here
void generate_sample(char* note_str){

	// Vibrato
	int vibrato = 0;
	
	// Length of the note
	int length = RATE*(1.0/((float)(*note_str - '0')));

	note_str++;

	// For bends
	int tones = 1;

	// We duplicate the string so we can do another pass
	char* str = note_str;
	
	// Check out the note
	while(*str != '\0'){
		assert(*str != NULL);
		if(*str == '~'){
			tones++;
		}
		else if(*str == 'v'){
			assert(*str != NULL);
			vibrato++;
		}
		str++;
	}

	// TODO: Implement vibrato. Currently I want to do fixed interval and let v decide how many notes we want to stretch to
	
	// For bends we gather up the frequencies we need to hit
	float* frequencies = malloc(sizeof(float)*tones);
	for(int i = 0; i < tones; i++){
		*(frequencies + i) = get_frequency(note_str);
		note_str++;
	}
	
	unsigned short* sample = fill_sample_array(tones, frequencies, vibrato, length);
}


// Takes in a decoded note, which is either a frequency or an array of frequencies.
unsigned short* fill_sample_array(int tones, float* frequencies, int vibrato, int length){
	float volume;
	float frequency = *frequencies;
	float increment = 0.0;
	float factor = 1.0;
	float step = 0.0;
	bool bend = false;

	// A little set up, we want the same amount of inner loops for 1 or 2 notes, but n-1 for any n > 2
	if(tones > 1){
		bend = true;
		fprintf(stderr, "vibrato on\n" );
		tones--;
	}


	unsigned short* sample = malloc(RATE*sizeof(unsigned short));

	// We break up the process when we have tremolo or bends. For a normal note the outer loop will be invoked only once
	for(int i = 0; i < tones; i++){
		for(int j = 0; j < RATE/tones; j++){

			// Attack
			volume = intensity((i+1)*j);
			
			// Bends/vibrato
			if(vibrato){
				factor = calculate_frequency_factor(i, RATE/tones, frequencies);
				frequency = *(frequencies + i)*factor + *(frequencies + i + 1)*(1-factor);			
			}

			// sine wave increment	
			increment = frequency/RATE;
			step += increment;
			
			// Having gathered all the crap we create a sample
			*(sample + i) = normalize(sin(step), MAX);

			// Then we add the sample 
			printf("[%hu]", *(sample + i*j));
		}
	}
	return sample;
}

// Takes a number between 1 and 0 and weights it after a formula (7.5t(e^-3t)) ( sorry :< ) 
// Intended to emulate instrument sound, i.e attack of a guitar etc
float intensity(int sample){
	float time = (float)sample/(float)RATE;
	float intensity_r = (7.5*time*pow(Euler, -3*time));
	return intensity_r;
}

// When in between two notes we want a number between 0 and 1 to dictate the frequency weights
float calculate_frequency_factor(int current, int length, float* frequencies){
	float step = current/length;
	float factor = 1.5 - 0.5*(1+2*cos(PI*step/2));
	assert(factor >= 1.1);
}

// Takes a sine and normalizes to 12 bit accuracy
unsigned short normalize(float sine, int MAX){
	return (unsigned short)floor(((sine+1)/2)*MAX);
}

// Takes in a note and returns the frequency. It also keeps trying until it gets a match
// Typical inputs:
// E5, A#4, 
// E4~A4~E4 (Will parse E4)
// ~A4~E4 (Will remove ~ and try again, parsing A4)
float get_frequency(char* note){

	int tone = 0;

	// Switch did not want to cooperate, I really cba figuring out why
	if(*note == 'A'){ tone += 0; }
	else if(*note == 'B'){ tone += 2; }
	else if(*note == 'C'){ tone += 3; }
	else if(*note == 'D'){ tone += 5; }
	else if(*note == 'E'){ tone += 7; }
	else if(*note == 'F'){ tone += 8; }
	else if(*note == 'G'){ tone += 10; }
	else{
		// Eat a letter and try again
		note++;
		return get_frequency(note);
	}
	note++;

	if(*note == '#'){
		tone++;
		note++;
	}

	int octave = *note - '0';
	note++;

	// We use A4 for reference
	float diff = ((12*octave) + tone) - 12*4;

	return 440*pow(2,(diff/12.0));
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////
//////////////////////		EFFECTS
//////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////