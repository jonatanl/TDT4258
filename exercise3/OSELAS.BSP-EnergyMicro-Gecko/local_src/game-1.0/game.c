#include <fcntl.h> 
#include <signal.h>
#include <poll.h>
#include <unistd.h> // getpid()
#include <errno.h>  // errno
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // uint16_t

// Hack to fix conflicting 'struct flock' declarations
#define HAVE_ARCH_STRUCT_FLOCK
#include <asm-generic/fcntl.h> // F_SETOWN

// Path to device file
#define DEVICE_PATH "/dev/gamepad"

// Global variables
static int devfd; // device file descriptor
static int error; // error variable


void signal_handler(int signal)
{
  // TODO: Implement a signal handler
}


void init_game()
{
  // Open the gamepad device read-write with nonblocking IO
  printf("game: Opening gamepad device %s ...\n", DEVICE_PATH);
  devfd = open(DEVICE_PATH, O_RDWR | O_NONBLOCK);
  if(devfd == -1)
    printf("Error opening gamepad device: %d\n", errno);

  // Set signal handler for the SIGIO signal
  signal(SIGIO, &signal_handler);

  // This process should receive SIGIO (and SIGURG) signals for events on the device file
  fcntl(devfd, F_SETOWN, getpid());

  // Enable asynchronous notification on the device file
  int flags = fcntl(devfd, F_GETFL) | FASYNC;
  fcntl(devfd, F_SETFL, flags);
}


void exit_game()
{
  // Close the gamepad device
  printf("game: Closing gamepad device ...\n");
  error = close(devfd);
  if(error == -1)
    printf("game: Error closing gamepad device: %d\n", errno);
}


int main(int argc, char *argv[])
{
  // TODO: Implement game loop

	exit(EXIT_SUCCESS);
}
