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
#include "draw.h"
#include "logic.h"
#include "input.h"

// Path to device file
#define DEVICE_PATH "/dev/gamepad"

#define NANOSECONDS_PER_FRAME (1000000000 / FRAMES_PER_SECOND)

// Function prototypes
int init_game(void);
int close_game(void);
void debug_test_run(void);
void sleep_to_next_frame(void);

// Global variables
static gamestate* my_gamestate;   // used to pass gamestates around
static int error;   // error variable

// Variables used to calculate frame sleep time
clockid_t my_clockid = CLOCK_MONOTONIC;
struct timespec time1;
struct timespec time2;
struct timespec sleep_time;
long int sleep_ns;

int main(int argc, char *argv[])
{
  // Initialize the game
  error = init_game();
  if(error){
    game_error("Error initializing the game. Exiting now.\n");
    exit(EXIT_FAILURE);
  }

  // Initialize sleep struct
  sleep_time.tv_sec = 0;
  sleep_time.tv_nsec = 0;

  // The game loop
  while(!error){
    game_debug("game loop started, calling sleep to next frame\n");
    sleep_to_next_frame(); 
    game_debug("starting clear all\n");
    clear_all();
    game_debug("starting do_logic\n");
    do_logic();
    game_debug("starting draw_all\n");
    draw_all();
    game_debug("starting update_display\n");
    update_display();
    game_debug("game loop ended\n");
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

// Compute the remaining nanoseconds until next frame and sleep.
//
// NOTE: We would prefer using clock_nanosleep(), but we could not get it to
// work. Maybe it's not defined in our version of the C library.
#define NANOSECONDS_PER_SECOND 1000000000
void sleep_to_next_frame(void)
{
  //clock_nanosleep(my_clockid, 0, &time1, &time2); 

  // End of current frame
  clock_gettime(my_clockid, &time1);

  // Initial sleep time
  sleep_ns = NANOSECONDS_PER_FRAME;

  // Subtract time spent in computation
  sleep_ns -= (time1.tv_nsec - time2.tv_nsec);
  sleep_ns -= (time1.tv_sec  - time2.tv_sec) * NANOSECONDS_PER_SECOND;

  // Skip missed frames
  while(sleep_ns < 0){  
    sleep_ns += NANOSECONDS_PER_FRAME;
    game_debug("(frame skipped!)\n");
  }
  sleep_ns = NANOSECONDS_PER_FRAME - sleep_ns;

  // Sleep until the next frame. Reuses time1 and time2.
  time1.tv_sec  = 0;
  time1.tv_nsec = sleep_ns;
  while(nanosleep(&time1, &time2)){

    if(errno == EINTR){

      // Interrupted by signal: Decrease sleep time and sleep again
      game_debug("(frame sleep interrupted)\n");
      time1.tv_nsec = time2.tv_nsec;
      continue;
    }else{

      // Any other error, such as invalid input: Exit game
      game_error("Unexpected error while sleeping for next frame: %s\n", strerror(errno));
      error = -1;
      break; 
    }
  }

  // Start of next frame
  clock_gettime(my_clockid, &time2);
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

  // Initialize frame sleep
  sleep_time.tv_sec  = 0;
  sleep_time.tv_nsec = 0;
  clock_gettime(my_clockid, &time2);

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
