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
#include "input.h"
#include "logic.h"
#include "util.h"
	
int init_input();
void signal_handler(int signal);
uint8_t sanitize_input_buffer(uint8_t n_input, uint8_t* input_buffer);

#define DEVICE_PATH "/dev/gamepad"

// To keep track of whether a read is needed or not.
static uint8_t input_raw;
static uint8_t output = 0x00;

static int devfd; // device file descriptor
static int error; // error variable


// When a signal arrives input_raw is updated
void signal_handler(int signal){
  int count;
  
  count = read(devfd, (void*)&input_raw, sizeof(uint8_t));
  while(count > 0){
    output  = input_raw & (ROTATE_LEFT | ROTATE_RIGHT | ACCELERATE);
    output |= input_raw & SHOOT;
    count = read(devfd, (void*)&input_raw, sizeof(uint8_t));
  }
}

uint8_t get_input(){
  uint8_t output = input_raw;
  input_raw = input_raw & (~SHOOT); 
  return output;
}

int init_input(){
  game_debug("Initializing the input module ...\n");

  // Open the gamepad device read-write with nonblocking IO
  devfd = open(DEVICE_PATH, O_RDONLY | O_NONBLOCK);
  if(devfd < 0){
    game_error("Error opening gamepad device at %s: %s\n", DEVICE_PATH, strerror(errno));
    return -1; // TODO: Handle error
  }
  game_debug("OK: Opened gamepad device at %s\n", DEVICE_PATH);

  // Set signal handler for the SIGIO signal
  if(signal(SIGIO, &signal_handler) == SIG_ERR){
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
  game_debug("DONE: No errors initializing the input module\n");
  return 0;
}

int release_input()
{
  game_debug("Releasing the input module ...\n");

  // Close the gamepad device
  error = close(devfd);
  if(error < 0){
    game_error("Error closing gamepad device: %s\n", strerror(errno));
    return -1; // TODO: Handle error
  }
  game_debug("OK: Gamepad device closed\n");

  // No errors
  game_debug("DONE: No errors releasing the input module\n");
  return 0;
}
