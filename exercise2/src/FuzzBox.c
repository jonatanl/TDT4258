#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define PI 3.1415

int max = 8191;
unsigned short normalize(float sine, int max);
float get_frequency(char* note);
unsigned short* generate_sine_sample(int rate, float frequency);
unsigned short* generate_triangle_sample(int rate, float frequency);
unsigned short* generate_square_sample(int rate, float frequency);

// Creates an array of values between 0 and max. 
// Called with args note and rate

void main(int argc, char** argv){

	time_t t;
	srand((unsigned) time(&t));

	if(argc < 2){
		printf("Missing args, use -A5 -44100 to generate a second of A5 at 44100 Hz\n");
		printf("SEG FAULT\n");
		printf("SEG FAULT\n");
		printf("SEG FAULT\n");
		printf("SEG FAULT\n");
		printf("SEG FAULT\n");
		printf("SEG FAULT\n");
	}
	// Get sample rate
	int rate = atoi(*(argv+2)+1);

	// Get frequency
	float frequency = get_frequency(*(argv+1)+1);
	
	// Generate a sample
	unsigned short* sample = generate_sine_sample(rate, frequency);
	free(sample);
	return;
}

unsigned short* generate_sine_sample(int rate, float frequency){
	float increment = (frequency/rate)*2*PI;
	unsigned short* sample = malloc(rate*sizeof(unsigned short));
	for(int i = 0; i < rate; i++){
		*(sample + i) = normalize(sin(i*increment), max);
	}
	return sample;
}

unsigned short* generate_square_sample(int rate, float frequency){
	float increment = (frequency/rate)*2*PI;
	unsigned short* sample = malloc(rate*sizeof(unsigned short));
	for(int i = 0; i < rate; i++){
		if(sin(i*increment) > 0){
			*(sample + i) = normalize(-1.0, max);	
		}
		else{
			*(sample + i) = normalize(1.0, max);
		}
	}
	return sample;
}

unsigned short* generate_triangle_sample(int rate, float frequency){
	float increment = (frequency/rate)*2*PI;
	unsigned short* sample = malloc(rate*sizeof(unsigned short));
	float count = 0.0;
	for(int i = 0; i < rate; i++){
		*(sample + i) = normalize(count, max);
		count += increment;
		if (count > 1.0){ count = 0; }
	}
	return sample;
}

// Normalizes to 12 bit accuracy
unsigned short normalize(float sine, int max){
	return (unsigned short)floor(((sine+1)/2)*max);
}

// Adds fuzz!
// Maybe try intensity around 1 - 10?
void fuzz_pedal(unsigned short* sample, int intensity, int rate){
	for(int i = 0; i < rate; i++){
		*(sample + i) *= (((float)(rand()%2) - 0.5)/2.0)*((float)(rand()%10)*0.01*intensity);
	}
}

float get_frequency(char* note){

	int tone = 0;
	if(*note == '#'){
		tone++;
		note++;
	}

	// Switch did not want to cooperate, I really cba figuring out why
	if(*note == 'A'){ tone += 0; }
	else if(*note == 'B'){ tone += 2; }
	else if(*note == 'C'){ tone += 3; }
	else if(*note == 'D'){ tone += 5; }
	else if(*note == 'E'){ tone += 7; }
	else if(*note == 'F'){ tone += 8; }
	else if(*note == 'G'){ tone += 10; }
	else{
		printf("Formatting error");
	}
	note++;
	int octave = *note - '0';

	// We use A4 for reference
	float diff = ((12*octave) + tone) - 12*4;

	return 440*pow(2,(diff/12.0));
}

