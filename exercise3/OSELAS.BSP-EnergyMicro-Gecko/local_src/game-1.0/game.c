// Set POSIX compliance.
//
// NOTE: Must be defined before including any header files!
#define _POSIX_C_SOURCE 200809L

#include <fcntl.h> 
#include <signal.h>
#include <poll.h>
#include <unistd.h> // getpid()
#include <errno.h>  // errno
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // uint16_t
#include <stdbool.h>// true, false
#include <string.h>
#include <time.h>         // clock_nanosleep(), clock_gettime()

#define DEBUG
#include "debug.h"
#include "logic.h"
#include "input.h"
#include "draw.h"

// Path to device file
#define DEVICE_PATH "/dev/gamepad"

#define NANOSECONDS_PER_FRAME (1000000000 / FRAMES_PER_SECOND)

// Function prototypes
int init_game(void);
int close_game(void);
void debug_test_run(void);

// Global variables
static gamestate* my_gamestate; // used to pass gamestates around
static int error; // error variable

//// The system clock used for waiting. CLOCK_MONOTONIC_RAW is not affected by
//// adjustments to system clocks, except NTP and adjtime.
clockid_t clock_id;
struct timespec next_frame; // the time of the next frame
struct timespec remaining;

int main(int argc, char *argv[])
{
  // Initialize the game
  error = init_game();
  if(error){
    game_error("Error initializing the game. Exiting now.\n");
    exit(EXIT_FAILURE);
  }

  // The game loop
  while(!error){
    do_logic();
    draw_all();
    update_display();

    // Sleep until the next frame.
    //
    // NOTE: We would prefer using clock_nanosleep(), but it doesn't seem to be
    // defined in our version of the c standard library.
    next_frame.tv_sec = 0;
    next_frame.tv_nsec = 0;//NANOSECONDS_PER_FRAME;
    while(nanosleep(&next_frame, &remaining)){

      if(errno == EINTR){

        // Interrupted by signal: Decrease sleep time and sleep again
        game_debug("(frame sleep interrupted)\n");
        next_frame.tv_nsec = remaining.tv_nsec;
        continue;
      }else{

        // Any other error: Exit game
        game_error("Unexpected error while sleeping for next frame: %s\n", strerror(errno));
        error = -1;
        break; 
      }
    }
  }

  // Close the game
  error = close_game();
  if(error){
    game_error("Error closing the game. Exiting now.\n");
    exit(EXIT_FAILURE);
  }

  // Exit successfully
	exit(EXIT_SUCCESS);
}

// Initialize game modules
int init_game()
{
  game_debug("Initializing the game ...\n");

  if((error = init_logic(&my_gamestate))){
    game_error("Error initializing logic module\n");
    return -1;
  }

  if((error = init_input())){
    game_error("Error initializing input module\n");
    return -1;
  }

  if((error = init_draw(my_gamestate))){
    game_error("Error initializing draw module\n");
    return -1;
  }

  // No errors
  game_debug("DONE: No errors initializing the game\n");
  return 0;
}

// Release game modules
int close_game()
{
  game_debug("Closing the game ...\n");

  if((error = release_logic())){
    game_error("Error releasing logic module\n");
    return -1;
  }

  if((error = release_input())){
    game_error("Error releasing input module\n");
    return -1;
  }

  if((error = release_draw())){
    game_error("Error releasing draw module\n");
    return -1;
  }

  // No errors
  game_debug("DONE: No errors initializing the game\n");
  return 0;
}

////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////
////////////    DEBUG
////////////
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////

void debug_test_run(void){
  // From util.h
  uint8_t test_inputs[] = {ROTATE_LEFT, ROTATE_RIGHT, ROTATE_RIGHT, ROTATE_RIGHT, ACCELERATE, 0};
  int test_inputs_len = 6;
  for(int i = 0; i < test_inputs_len; i++){
    do_logic_input(test_inputs[i]);
    draw_all();
    update_display();  
  }
  // Crash the program
  int a = 0;
  int b = 0;
//  int *a = b;
}
