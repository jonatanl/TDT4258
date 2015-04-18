#include "input.h"
#include "logic.h"
#include "util.h"
#include <fcntl.h> 
#include <signal.h>
#include <poll.h>
#include <stdbool.h>
#include <unistd.h> // getpid()
#include <errno.h>  // errno
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // uint16_t
#include <string.h>

#define HAVE_ARCH_STRUCT_FLOCK
#include <asm-generic/fcntl.h>

#define DEBUG
#include "debug.h"
	
int init_input();
void signal_handler(int signal);
uint8_t sanitize_input_buffer(uint8_t n_input, uint8_t* input_buffer);

#define DEVICE_PATH "/dev/gamepad"

// To keep track of whether a read is needed or not.
static uint8_t input_feed = 0;

static int devfd; // device file descriptor
static int error; // error variable

int init_input(){
  // TODO: Fix ouput statements
  // Open the gamepad device read-write with nonblocking IO
  devfd = open(DEVICE_PATH, O_RDONLY | O_NONBLOCK);
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


int teardown_input()
{
  // TODO: Fix ouput statements
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


// When a signal arrives input_feed is updated
void signal_handler(int signal){
  int count = read(devfd, (void*)&input_feed, sizeof(uint8_t));
  if(count){
    game_debug("read recieved input\n");
  }
  game_debug("read recieved no input\n");
}

uint8_t get_input(){
  return ~input_feed;
}
