#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h> // write(), close()
#include <sys/stat.h>  //
#include <fcntl.h>     //

#include <unistd.h>    // open()

#include <errno.h>     // errno macro

#include <stdint.h>    // uint16_t

#include <sys/mman.h>  // mmap(), munmap()

#include <linux/fb.h>  // struct fb_copyarea

#include <sys/ioctl.h> // ioctl()

#define NUM_PIXELS (320 * 240) // Number of frame buffer pixels
#define BUF_SIZE (NUM_PIXELS * sizeof(uint16_t)) // Size of the frame buffer

static int error; // Error variable


int main(int argc, char *argv[])
{
  // Open frame buffer write-only
  //
  // NOTE: O_WRONLY is not sufficient for mmap()
  int fbfd = open("/dev/fb0", O_RDWR);
  if(fbfd == -1)
    printf("Error opening file: %d\n", errno);

  // Map the frame buffer file to memory
  uint16_t *framebuf = (uint16_t*)mmap(
      NULL,                   // kernel decides physical memory address
      BUF_SIZE,               // frame buffer size
      PROT_READ | PROT_WRITE, // memory is read-write
      MAP_SHARED,             // memory is shared with other processes
      fbfd,                     // the mapped file
      0);
  if(framebuf == (void*)-1)
    printf("Error mapping file to memory: %d\n", errno);

  // Color every second pixel yellow
  for(int i=0; i<NUM_PIXELS; i+=2){
    framebuf[i] = (31 << 11) + (63 << 5) + (0 << 0);
  }

  // Synchronize file with memory map
  //
  // MS_SYNC: Wait until done
  error = msync(framebuf, BUF_SIZE, MS_SYNC);
  if(error == -1)
    printf("Error synchronizing file with memory map: %d\n", errno);

  // Refresh screen area
  struct fb_copyarea rect;
  rect.dx = 0;
  rect.dy = 0;
  rect.width = 320;
  rect.height = 240;
  ioctl(fbfd, 0x4680, &rect);

  // Delete framebuffer mapping
  error = munmap((void*)framebuf, BUF_SIZE);
  if(error == -1)
    printf("Error deleting mapping: %d\n", errno);

  // Close framebuffer file
  error = close(fbfd);
  if(error == -1)
    printf("Error writing to file: %d\n", errno);

  printf("Every second pixel should now be yellow. Can you still see Tux?\n");
	exit(EXIT_SUCCESS);
}
