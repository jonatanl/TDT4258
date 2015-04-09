#include <sys/types.h> // write(), open(), close()
#include <sys/stat.h>  //
#include <fcntl.h>     //
#include <unistd.h>    // pread(), fcntl(), getpid()
#include <signal.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>    // uint16_t
#include <errno.h>     // errno
#include <poll.h>

// Hack to fix conflicting 'struct flock' declarations
#define HAVE_ARCH_STRUCT_FLOCK
#include <asm-generic/fcntl.h> // F_SETOWN


#define DEVICE_PATH "/dev/gamepad"


int devfd;  // device file descriptor
static int done = 0;  // set to 1 when application should exit
static int error;     // error variable


// a simple signal handler
void signal_handler(int signal){
  uint8_t value;
  uint32_t count;

  printf("game: Signal received!\n");


  // create a pollfd structure used to poll the device file for available data
  struct pollfd pollfd = {
    .fd = devfd,
    .events = POLLIN,
  };

  // poll the device for available data
  printf("game: polling the device ... ");
  poll(&pollfd, // array of all pollfd structures to wait for (only one) 
      1,        // array length
      0);       // timeout: 0 -> may return immediately
  if(pollfd.revents == POLLIN){
    printf("data is available!\n");
  }else{
    printf("something else happened. Timeout?\n");
  }

  // read a value and print it
  printf("game: reading data ... ");
  count = read(devfd, (void*)&value, sizeof(uint8_t)); // this call might block
  if(count > 0){
    printf("got %d bytes of input: %d\n", count, value);
    if(value >= 128){
      done = 1; // exit the application
    }
  }else{
    printf("got no data. Error?\n");
  }
}


int main(int argc, char *argv[])
{
  // Open device read-write
  printf("game: Trying to open the gamepad at %s ...\n", DEVICE_PATH);
  devfd = open(DEVICE_PATH, O_RDWR);
  if(devfd == -1)
    printf("Error opening file: %d\n", errno);

  // Set signal handler for the SIGIO signal
  signal(SIGIO, &signal_handler);

  // This process should receive SIGIO (and SIGURG) signals for events on the device file
  fcntl(devfd, F_SETOWN, getpid());

  // Enable asynchronous notification on the device file
  int flags = fcntl(devfd, F_GETFL) | FASYNC;
  fcntl(devfd, F_SETFL, flags);

  // Spin until a SIGIO signal is received
  printf("game: Waiting for button input ... (Press Button 8 to exit)\n");
  while(!done){
    // Do nothing
  }

  // Close framebuffer file
  printf("game: Trying to close the gamepad ...\n");
  error = close(devfd);
  if(error == -1)
    printf("game: Error closing file: %d\n", errno);

	exit(EXIT_SUCCESS);
}
