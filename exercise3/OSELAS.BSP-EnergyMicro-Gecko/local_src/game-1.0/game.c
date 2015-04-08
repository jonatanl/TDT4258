#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h> // write(), close()
#include <sys/stat.h>  //
#include <fcntl.h>     //

#include <unistd.h>    // open()

#include <errno.h>     // errno

#include <stdint.h>    // uint16_t
#include <unistd.h>    // pread()

#define DEVICE_PATH "/dev/gamepad"

static int error; // Error variable


int main(int argc, char *argv[])
{
  // Open device read-write
  printf("game: Trying to open the gamepad at %s ...\n", DEVICE_PATH);
  int devfd = open(DEVICE_PATH, O_RDWR);
  if(devfd == -1)
    printf("Error opening file: %d\n", errno);

  // Read from device
  printf("game: Waiting for button input ... (Press Button 8 to exit)\n");
  int value;
  int count;
  while(1){
    count = read(devfd, (void*)&value, sizeof(uint8_t));
    if(count){
      printf("game: got input: %d\n", value);
      if(value >= 128){
        break;
      }
    }
  }

  // Close framebuffer file
  printf("game: Trying to close the gamepad ...\n");
  error = close(devfd);
  if(error == -1)
    printf("game: Error closing file: %d\n", errno);

	exit(EXIT_SUCCESS);
}
