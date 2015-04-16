#include <fcntl.h>  // open()
#include <unistd.h> // close()
#include <string.h> // strerror()

#define DEBUG
#include "debug.h"
#include "draw.h"

#define FB_DEVICE_FILE "/dev/fb0"

static int fbfd;
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

  // No errors
  game_debug("DONE: No errors initializing the draw module\n");
  return 0;
}

void teardown_draw()
{
  game_debug("Releasing draw module resources ...\n");

  // Close framebuffer device
  error = close(fbfd);
  if(error < 0){
    game_error("Error closing framebuffer device: %s\n", strerror(errno));
  }
  game_debug("OK: Closed framebuffer device\n");

  // No errors
  game_debug("DONE: No errors releasing module resources\n");
  return;
}
