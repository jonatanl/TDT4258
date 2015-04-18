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
#include <string.h>

#define DEBUG
#include "debug.h"
#include "draw.h"
#include "input.h"

// Path to device file
#define DEVICE_PATH "/dev/gamepad"

// Global variables
static gamestate* my_gamestate; // used to pass gamestates around
static int devfd; // device file descriptor
static int error; // error variable

int init_game()
{
  // TODO
  return 0;
}

int close_game()
{
  // TODO
  return 0;
}


int main(int argc, char *argv[])
{
  // Initialize the game
  error = init_game();
  if(error){
    game_error("Error initializing game. Exiting now.\n");
    exit(EXIT_FAILURE);
  }

  ///////////////
  // TEST CODE:
  ///////////////
  
  // Initialize logic module
  my_gamestate = init_logic();

  // Initialize draw module
  error = init_draw(my_gamestate);

  draw_all();
  update_display();

  // TODO: Implement game loop
  
  // Release draw module resources
  teardown_draw();

  // TODO: Release module resources

  // Close the game
  error = close_game();
  if(error){
    game_error("Error closing game. Exiting now.\n");
    exit(EXIT_FAILURE);
  }

  // Exit successfully
	exit(EXIT_SUCCESS);
}
