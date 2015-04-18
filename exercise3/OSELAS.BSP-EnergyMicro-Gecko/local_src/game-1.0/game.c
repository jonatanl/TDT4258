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

int close_game()
{
  game_debug("Closing the game ...\n");

  // Close the gamepad device
  error = close(devfd);
  if(error < 0){
    game_error("Error closing gamepad device: %s\n", strerror(errno));
    return -1; // TODO: Handle error
  }
  game_debug("OK: Gamepad device closed\n");

  // No errors
  game_debug("DONE: No errors closing the game\n");
  return 0;
}


int main(int argc, char *argv[])
{
  // Initialize the game
  //error = init_game();
  //if(error){
  //  game_error("Error initializing game. Exiting now.\n");
  //  exit(EXIT_FAILURE);
  //}

  // Initialize draw module
  // TODO: Initialize the gamestate first!
  my_gamestate = init_logic();
  error = init_draw(my_gamestate);

  // TODO: Implement game loop
  
  // Release draw module resources
  teardown_draw();

  // Close the game
  error = close_game();
  if(error){
    game_error("Error closing game. Exiting now.\n");
    exit(EXIT_FAILURE);
  }

  // Exit successfully
	exit(EXIT_SUCCESS);
}
