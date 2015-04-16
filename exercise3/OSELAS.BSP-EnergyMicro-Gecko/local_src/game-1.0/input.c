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
uint8_t get_input();
void signal_handler(int signal);
uint8_t sanitize_input_buffer(uint8_t n_input, uint8_t* input_buffer);

#define DEVICE_PATH "/dev/gamepad"

// To keep track of whether a read is needed or not.
static uint16_t new_input = 0;
static uint8_t input_feed = 0;

static int devfd; // device file descriptor
static int error; // error variable

int init_input(){
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

// When a signal arrives a flag for input read is set
void signal_handler(int signal){new_input++;}

// Reads a single input and sanitizes it
uint8_t read_input(){
  // TODO implement actual read
  uint8_t some_input = 8;
  return sanitize_input(some_input);
}

// Just a demo, makes sure if left and rght turn is held down right is always preferred
uint8_t sanitize_input(uint8_t raw_input){
  if(CHECK_LEFT(raw_input) && CHECK_RIGHT(raw_input)){
    return new_input = FLIP_LEFT(raw_input);
  }
  return raw_input;
}

// Reads a list of buffers
uint8_t read_input_buffer(){
  // TOTO implement actual buffer read
  uint8_t n_input_buffer = 4;
  uint8_t* input_buffer = malloc(sizeof(uint8_t)*n_input_buffer);
  return sanitize_input_buffer(n_input_buffer, input_buffer);
}

// TODO reimplement with actual details
uint8_t sanitize_input_buffer(uint8_t n_input, uint8_t* input_buffer){
  for(int i = 0; i < n_input; i++){
    // TODO some cool logic sanitizing input
  }
}

// Modifies input_feed if necessary and returns it.
uint8_t get_input(){
  if(new_input){
    new_input = 0;
    input_feed = read_input();
  }
  return input_feed;
}
