// Hack to fix conflicting 'struct flock' declarations
#define HAVE_ARCH_STRUCT_FLOCK
#include <asm-generic/fcntl.h> // F_SETOWN

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

#include "logic.h"
#include "input.h"
#include "draw.h"
#define DEBUG
#include "debug.h"

// Path to device file
#define DEVICE_PATH "/dev/gamepad"

// Global variables
static gamestate* my_gamestate; // used to pass gamestates around
static int error; // error variable

// Function prototypes
int init_game();
int close_game();

int main(int argc, char *argv[])
{
  // Initialize the game
  error = init_game();
  if(error){
    game_error("Error initializing the game. Exiting now.\n");
    exit(EXIT_FAILURE);
  }

  // The game loop
  // TODO: Implement it
  while(true){
    draw_all();
    update_display();
    break;
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
