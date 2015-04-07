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
  printf("Trying to open the gamepad at %s ...\n", DEVICE_PATH);
  int devfd = open(DEVICE_PATH, O_RDWR);
  if(devfd == -1)
    printf("Error opening file: %d\n", errno);

  // Read from device
  printf("Waiting for button input ... (Press Button 8 to exit)\n");
  int value = 0;
  int prev_value = 0;
  while(1){
    read(devfd, (void*)&value, sizeof(uint8_t));
    if(value != prev_value){
      printf("GPIO_PC_DIN: %d\n", value);
      prev_value = value;
      if(value >= 128){
        break;
      }
    }
  }

  // Close framebuffer file
  printf("Trying to close the gamepad ...\n");
  error = close(devfd);
  if(error == -1)
    printf("Error closing file: %d\n", errno);

	exit(EXIT_SUCCESS);
}
