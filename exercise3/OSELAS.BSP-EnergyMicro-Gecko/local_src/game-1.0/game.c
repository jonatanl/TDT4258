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

// Path to device file
#define DEVICE_PATH "/dev/gamepad"

// Global variables
static gamestate my_gamestate; // used to pass gamestates around
static int devfd; // device file descriptor
static int error; // error variable

void signal_handler(int signal)
{
  // TODO: Implement a signal handler
}


int init_game()
{
  game_debug("Initializing the game ...\n");

  // Open the gamepad device read-write with nonblocking IO
  devfd = open(DEVICE_PATH, O_RDWR | O_NONBLOCK);
  if(devfd < 0){
    game_error("Error opening gamepad device at %s: %s\n", DEVICE_PATH, strerror(errno));
    return -1; // TODO: Handle error
  }
  game_debug("OK: Gamepad device opened at %s\n", DEVICE_PATH);

  // Set signal handler for the SIGIO signal
  error = signal(SIGIO, &signal_handler);
  if(error == SIG_ERR){
    game_error("Error setting a signal handler: %s\n", strerror(errno));
    return -1;
  }
  game_debug("OK: Set signal handler for SIGIO\n");

  // This process should receive SIGIO (and SIGURG) signals for events on the device file
  error = fcntl(devfd, F_SETOWN, getpid());
  if(error < 0){
    game_error("Error setting owner of SIGIO and SIGURG signals: %s\n", strerror(errno));
    return -1;
  }
  game_debug("OK: Set owner for SIGIO signals on gamepad device\n");

  // Enable asynchronous notification on the device file
  int flags = fcntl(devfd, F_GETFL) | FASYNC;
  error = fcntl(devfd, F_SETFL, flags);
  error = fcntl(devfd, F_SETOWN, getpid());
  if(error < 0){
    game_error("Error setting asynchronous notification on device: %s\n", strerror(errno));
    return -1;
  }
  game_debug("OK: Enabled asynchronous notification for gamepad device\n");

  // No errors
  game_debug("DONE: No errors initializing the game\n");
  return 0;
}


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
  error = init_game();
  if(error){
    game_error("Error initializing game. Exiting now.\n");
    exit(EXIT_FAILURE);
  }

  // Initialize draw module
  // TODO: Initialize the gamestate first!
  error = init_draw(&my_gamestate);

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
