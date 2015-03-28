#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h> // write(), close()
#include <sys/stat.h>  //
#include <fcntl.h>     //

#include <unistd.h>    // open()

#include <errno.h>     // errno macro

#include <stdint.h>    // uint16_t

#define DEVICE_PATH "/dev/driver-gamepad"

static int error; // Error variable


int main(int argc, char *argv[])
{
  // Open device read-write
  printf("Trying to open the gamepad at %s ...\n", DEVICE_PATH);
  int devfd = open(DEVICE_PATH, O_RDWR);
  if(devfd == -1)
    printf("Error opening file: %d\n", errno);

  // Close framebuffer file
  printf("Trying to close the gamepad ...\n");
  error = close(devfd);
  if(error == -1)
    printf("Error closing file: %d\n", errno);

	exit(EXIT_SUCCESS);
}
