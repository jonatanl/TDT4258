#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <stdbool.h>

#define PI 3.1415
#define Euler 2.7182

float get_frequency(int note);
int decode(char* note);
unsigned short normalize(float sine, int MAX);
unsigned short* fill_sample_array(float frequency);


// These define the sampling rate and maximum amplitude
int RATE;
int MAX;
int SAMPLE_LEN;

void main(int argc, char** argv){

	SAMPLE_LEN = 4410;
	MAX = 4095;
	RATE = 44100;

	int total_notes = 0;

	unsigned short* samples[100];	// This is the array of samples we will use
	for(int i = 0; i < 100; i++){
		samples[i] = NULL;
	}

	int note_addresses[100]; 		// Used to keep track off added notes

	printf("#include <stdint.h>\n");
	printf("uint16_t* note_array[100];");


	// We decode the notes and add them to the table
	int input = 1;
	while(input != EOF){



		// Get input
		char input_string [50];
		input = scanf("%50s", input_string);
		if(input == EOF){
			break;
		}

		// Handle input

		// Get a note from a string
		int note = decode(input_string);

		fprintf(stderr, "Got note %d\n", note);

		// We make sure to not generate duplicates
		if(samples[note] == NULL){

			printf("uint16_t note%d[%hu] = {", total_notes, RATE);

			// Get a frequency from a note
			float frequency = get_frequency(note);

			// Get a sample from the frequency
			unsigned short* sample = fill_sample_array(frequency);

			samples[note] = sample;

			for(int i = 0; i < SAMPLE_LEN - 1; i++){
				printf("%hu,", sample[i]);
			}
			printf("0};\n\n");

			printf("note_array[%d] = note%d;\n", note, total_notes);

			note_addresses[total_notes] = note;
			total_notes++;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////
///////////////////
///////////////////
///////////////////
///////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int decode(char* note){

	int tone = 0;

	// Switch did not want to cooperate, so a less elegant solution will have to do
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
		return decode(note);
	}
	note++;

	if(*note == '#'){
		tone++;
		note++;
	}

	int octave = *note - '0';
	note++;

	return 12*octave + tone;
}

float get_frequency(int note){
	float diff = (note) - 12*4;
	float frequency = 440*pow(2,(diff/12.0));

	return frequency;
}

// Generates a sample of length SAMPLE_LEN
unsigned short* fill_sample_array(float frequency){
	
	fprintf(stderr, "Allocating %d blocks for ushort\n", SAMPLE_LEN);
	unsigned short* sample = malloc(SAMPLE_LEN*sizeof(unsigned short));
	float increment = frequency/RATE;
	float step = 0.0;

	int testc = 0;

	for(int i = 0; i < SAMPLE_LEN; i++){
		step += increment;
		unsigned short test_var = normalize(sin(step), MAX);
		sample[i] = test_var;
	}

	// Prints sample:
	//for(int i = 0; i < SAMPLE_LEN; i++){
	//	fprintf(stderr, "%d: [%hu]\n", i, sample[i] );
	//}

	return sample;
}

unsigned short normalize(float sine, int MAX){

	unsigned short normalized = (unsigned short)floor(((sine+1)/2)*MAX);
	return normalized;
}