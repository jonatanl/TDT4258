#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h> // write(), close()
#include <sys/stat.h>  //
#include <fcntl.h>     //

#include <unistd.h>    // open()

#include <errno.h>     // errno macro

#include <stdint.h>    // uint16_t

#define BUF_SIZE (320 * 120) // Buffer covers half the screen

static int error; // Error variable

int main(int argc, char *argv[])
{
  // Open framebuffer write-only
  int fb = open("/dev/fb0", O_WRONLY);
  if(fb == -1)
    printf("Error opening file: %d\n", errno);

  // Generate an array of yellow pixels
  uint16_t *data = malloc(sizeof(uint16_t) * BUF_SIZE);
  for(int i=0; i<BUF_SIZE; i++){
    data[i] = (31 << 11) + (63 << 5) + (0 << 0);
  }

  // Write yellow pixels
  error = write(fb, (void*)data, sizeof(uint16_t) * BUF_SIZE);
  if(error == -1)
    printf("Error writing to file file: %d\n", errno);
	printf("Half the screen should be yellow now!\n");

  // Close framebuffer
  error = close(fb);
  if(error == -1)
    printf("Error writing to file file: %d\n", errno);

	exit(EXIT_SUCCESS);
}
