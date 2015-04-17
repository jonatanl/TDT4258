#include <fcntl.h>     // open()
#include <unistd.h>    // close()
#include <string.h>    // strerror()
#include <errno.h>     // errno
#include <sys/mman.h>  // mmap(), munmap()
#include <sys/ioctl.h> // ioctl()
#include <linux/fb.h>  // struct fb_copyarea

#define DEBUG
#include "debug.h"
#include "draw.h"

// Hard-coded framebuffer parameters
#define FB_DISPLAY_WIDTH  (320)
#define FB_DISPLAY_HEIGHT (240)
#define FB_DISPLAY_SIZE   (320 * 240)
#define FB_DEVICE_PATH    ("/dev/fb0")
#define FB_DEVICE_SIZE    (FB_DISPLAY_SIZE * sizeof(uint16_t))

// Precomputed color values
#define color(R,G,B) ((R << 11) + (G << 5) + (B << 0))
#define FB_COLOR_WHITE color(31, 63, 31) 
#define FB_COLOR_BLACK color( 0,  0,  0)
#define FB_COLOR_GRAY  color(15, 31, 15)
#define FB_COLOR_RED   color(31,  0,  0)
#define FB_COLOR_GREEN color( 0, 63,  0)
#define FB_COLOR_BLUE  color( 0,  0, 31)

// Function prototypes
void draw_line_octant1(int x, int y, int ex, int ey, int dx, int dy);
void draw_line_octant2(int x, int y, int ex, int ey, int dx, int dy);
void draw_line_octant3(int x, int y, int ex, int ey, int dx, int dy);
void draw_line_octant8(int x, int y, int ex, int ey, int dx, int dy);
void test_draw();

// Global variables
static int fbfd;
static uint16_t* fbmem;
static int error;
static struct fb_copyarea rect;
static uint16_t draw_color; // current color for drawing

int init_draw(struct gamestate* gamestate)
{
  game_debug("Initializing draw module ...\n");

  // Open framebuffer device with read-write permissions.
  //
  // NOTE: read-write is needed for mmap()
  fbfd = open(FB_DEVICE_PATH, O_RDWR);
  if(fbfd < 0){
    game_error("Error opening framebuffer device: %s\n", strerror(errno));
  }
  game_debug("OK: Opened framebuffer device %s\n", FB_DEVICE_PATH);

  // Map the framebuffer device to memory
  fbmem = (uint16_t*)mmap(
      NULL,                              // let kernel decide physical memory address
      FB_DISPLAY_SIZE * sizeof(uint16_t), // framebuffer size
      PROT_READ | PROT_WRITE,            // memory is read-write
      MAP_SHARED,                        // updates are carried immediately
      fbfd,                              // the mapped file
      0);                                // offset
  if(fbmem == MAP_FAILED){
    game_error("Error mapping file to memory: %s\n", strerror(errno));
    return -1;
  }
  game_debug("OK: Mapped framebuffer device to memory\n");

  test_draw(); // TODO: Remove this call in later implementations

  // No errors
  game_debug("DONE: No errors initializing the draw module\n");
  return 0;
}

int teardown_draw()
{
  game_debug("Releasing draw module resources ...\n");

  // Unmap framebuffer device from memory
  error = munmap((void*)fbmem, FB_DISPLAY_SIZE * sizeof(uint16_t));
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

static void inline set_pixel(int x, int y)
{
  fbmem[x + y * FB_DISPLAY_WIDTH] = FB_COLOR_GRAY;
}

void draw_line(int sx, int sy, int ex, int ey)
{
  do_draw_line(sx, sy, ex, ey, FB_COLOR_GRAY);
}

void clear_line(int sx, int sy, int ex, int ey)
{
  do_draw_line(sx, sy, ex, ey, FB_COLOR_BLACK);
}

// This function calls one of four line rasterization functions based on which
// octant the specified line resides in. Each function implements a version of
// the Bresenham line rasterization algorithm.
//
// NOTE: The algorithm was borrowed and adapted from:
//     Graphics & Visualization, Principles and Algorithms
// by:
//     T. Theoharis, G. Papaioannou, N. Platis, N. Patrikalakis
#define OCTANT_1  0
#define OCTANT_2  1
#define OCTANT_3  3
#define OCTANT_4  7
#define OCTANT_5 15
#define OCTANT_6 14
#define OCTANT_7 12
#define OCTANT_8  8
void do_draw_line(int sx, int sy, int ex, int ey, uint16_t color)
{
  int dx = ex - sx;
  int dy = ey - sy;
  int octant_code = 0;

  // set color
  draw_color = color;

  // identify the octant of the point (dx, dy)
  octant_code |= (dx - dy < 0) << 0;
  octant_code |= (dx < 0)      << 1; 
  octant_code |= (dx + dy < 0) << 2;
  octant_code |= (dy < 0)      << 3;
  
  // depending on the octant, draw the line
  switch(octant_code){
    case OCTANT_8:
      draw_line_octant8(sx, sy, ex, ey, dx, dy);
      break;
    case OCTANT_1:
      draw_line_octant1(sx, sy, ex, ey, dx, dy);
      break;
    case OCTANT_2:
      draw_line_octant2(sx, sy, ex, ey, dx, dy);
      break;
    case OCTANT_3:
      draw_line_octant3(sx, sy, ex, ey, dx, dy);
      break;
    case OCTANT_4:
      draw_line_octant8(ex, ey, sx, sy, -dx, -dy);
      break;
    case OCTANT_5:
      draw_line_octant1(ex, ey, sx, sy, -dx, -dy);
      break;
    case OCTANT_6:
      draw_line_octant2(ex, ey, sx, sy, -dx, -dy);
      break;
    case OCTANT_7:
      draw_line_octant3(ex, ey, sx, sy, -dx, -dy);
      break;
    default:
      game_error("Illegal octant code: %d\n", octant_code);
  }
}

void draw_line_octant1(int x, int y, int ex, int ey, int dx, int dy)
{
  // Leap decision variable keeps track of when to increment y.
  int e = -(dx >> 1);

  // Draw pixels. Stop when (ex, ey) is reached.
  while(x <= ex){
    set_pixel(x, y);
    
    // update current coordinate
    x += 1;
    e += dy;
    if(e >= 0){
      y += 1;
      e -= dx;
    }
  }
}

void draw_line_octant2(int x, int y, int ex, int ey, int dx, int dy)
{
  // Leap decision variable keeps track of when to increment y.
  int e = -(dy >> 1);

  // Draw pixels. Stop when (ex, ey) is reached.
  while(y <= ey){
    set_pixel(x, y);
    
    // update current coordinate
    y += 1;
    e += dx;
    if(e >= 0){
      x += 1;
      e -= dy;
    }
  }
}

void draw_line_octant3(int x, int y, int ex, int ey, int dx, int dy)
{
  // Leap decision variable keeps track of when to increment y.
  int e = -(dy >> 1);
  dx = -dx;

  // Draw pixels. Stop when (ex, ey) is reached.
  while(y <= ey){
    set_pixel(x, y);
    
    // update current coordinate
    y += 1;
    e += dx;
    if(e >= 0){
      x -= 1;
      e -= dy;
    }
  }
}

void draw_line_octant8(int x, int y, int ex, int ey, int dx, int dy)
{
  // Leap decision variable keeps track of when to increment y.
  int e = -(dx >> 1);
  dy = -dy;

  // Draw pixels. Stop when (ex, ey) is reached.
  while(x <= ex){
    set_pixel(x, y);
    
    // update current coordinate
    x += 1;
    e += dy;
    if(e >= 0){
      y -= 1;
      e -= dx;
    }
  }
}

void test_draw()
{
  game_debug("Running the drawing test ...\n");

  // a rectangle
  int rx = FB_DISPLAY_WIDTH  / 16;
  int ry = FB_DISPLAY_HEIGHT / 16;
  int rw = FB_DISPLAY_WIDTH  - (2 * rx);
  int rh = FB_DISPLAY_HEIGHT - (2 * ry);

  // center of the rectangle
  int cx = rx + (rw / 2);
  int cy = ry + (rh / 2);

  // clear the display
  for(int i=0; i<FB_DISPLAY_SIZE; i++){
    fbmem[i] = FB_COLOR_BLACK;
  }
  game_debug("OK: Removed Tux (some people have spheniscidaeaphobia)\n");

  // draw the rectangle
  draw_line(rx         , ry         , rx + rw - 1, ry         );
  draw_line(rx + rw - 1, ry         , rx + rw - 1, ry + rh - 1);
  draw_line(rx + rw - 1, ry + rh - 1, rx         , ry + rh - 1);
  draw_line(rx         , ry + rh - 1, rx         , ry         );

  // draw lines from the center of the rectangle to the edges
  int lines = 10;
  for(int i=0; i<lines; i++){
    draw_line(cx, cy, rx + (rw * i / lines), ry);
    draw_line(cx, cy, rx + (rw * i / lines), ry + rh - 1);
    draw_line(cx, cy, rx, ry + (rh * i / lines));
    draw_line(cx, cy, rx + rw - 1, ry + (rh * i / lines));
  }
  game_debug("OK: Wrote lines to the framebuffer\n");

  // update the display
  rect.dx = 0;
  rect.dy = 0;
  rect.width  = FB_DISPLAY_WIDTH;
  rect.height = FB_DISPLAY_HEIGHT;
  ioctl(fbfd, 0x4680, &rect);
  game_debug("OK: Updated the display\n");

  game_debug("DONE: Drawing test completed. Can you see lines on the display?\n");
}
