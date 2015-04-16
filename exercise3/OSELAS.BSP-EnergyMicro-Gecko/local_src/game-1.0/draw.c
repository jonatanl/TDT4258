#include <fcntl.h>    // open()
#include <unistd.h>   // close()
#include <string.h>   // strerror()
#include <errno.h>    // errno
#include <sys/mman.h> // mmap(), munmap()

#define DEBUG
#include "debug.h"
#include "draw.h"

#define FB_DEVICE_FILE "/dev/fb0"
#define FB_DEVICE_SIZE (320 * 240)

static int fbfd;
static uint16_t* fbmem;
static int error;

int init_draw(struct gamestate* gamestate)
{
  game_debug("Initializing draw module ...\n");

  // Open framebuffer device with read-write permissions.
  //
  // NOTE: read-write is needed for mmap()
  int fbfd = open(FB_DEVICE_FILE, O_RDWR);
  if(fbfd < 0){
    game_error("Error opening framebuffer device: %s\n", strerror(errno));
  }
  game_debug("OK: Opened framebuffer device %s\n", FB_DEVICE_FILE);

  // Map the framebuffer device to memory
  fbmem = (uint16_t*)mmap(
      NULL,                              // let kernel decide physical memory address
      FB_DEVICE_SIZE * sizeof(uint16_t), // framebuffer size
      PROT_READ | PROT_WRITE,            // memory is read-write
      MAP_SHARED,                        // updates are carried immediately
      fbfd,                              // the mapped file
      0);                                // offset
  if(fbmem == MAP_FAILED){
    game_error("Error mapping file to memory: %s\n", strerror(errno));
    return -1;
  }
  game_debug("OK: Mapped framebuffer device to memory\n");

  // No errors
  game_debug("DONE: No errors initializing the draw module\n");
  return 0;
}

int teardown_draw()
{
  game_debug("Releasing draw module resources ...\n");

  // Unmap framebuffer device from memory
  error = munmap((void*)fbmem, FB_DEVICE_SIZE * sizeof(uint16_t));
  if(error < 0){
    game_error("Error unmapping device file from memory: %s\n", strerror(errno));
    return -1;
  }
  game_error("OK: Unmapped device file from memory\n");

  // Close framebuffer device
  error = close(fbfd);
  if(error < 0){
    game_error("Error closing framebuffer device: %s\n", strerror(errno));
    return -1;
  }
  game_debug("OK: Closed framebuffer device\n");

  // No errors
  game_debug("DONE: No errors releasing module resources\n");
  return 0;
}
