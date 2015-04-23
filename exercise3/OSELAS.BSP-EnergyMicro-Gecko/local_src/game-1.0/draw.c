#include <fcntl.h>     // open()
#include <unistd.h>    // close()
#include <string.h>    // strerror()
#include <errno.h>     // errno
#include <sys/mman.h>  // mmap(), munmap()
#include <sys/ioctl.h> // ioctl()
#include <linux/fb.h>  // struct fb_copyarea
#include <stdlib.h> // absolute value abs()

#include "draw.h"
#include "game.h"
#define DEBUG
#include "debug.h"
#include "logic.h"

// Hard-coded framebuffer parameters
#define FB_DEVICE_PATH    ("/dev/fb0")
#define FB_DEVICE_SIZE    (DISPLAY_SIZE * sizeof(uint16_t))

// Precomputed color values
#define color(R,G,B) ((R << 11) + (G << 5) + (B << 0))
#define FB_COLOR_WHITE color(31, 63, 31) 
#define FB_COLOR_BLACK color( 0,  0,  0)
#define FB_COLOR_GRAY  color(15, 31, 15)
#define FB_COLOR_RED   color(31,  0,  0)
#define FB_COLOR_GREEN color( 0, 63,  0)
#define FB_COLOR_BLUE  color( 0,  0, 31)

// Function prototypes
void do_draw_all(void);
void do_draw_polyline(int32_t* x_coords, int32_t* y_coords, int n_points);
void do_draw_line(int sx, int sy, int ex, int ey);
void draw_line_octant1(int is, int ie, int dx, int dy);
void draw_line_octant2(int is, int ie, int dx, int dy);
void draw_line_octant3(int is, int ie, int dx, int dy);
void draw_line_octant8(int is, int ie, int dx, int dy);
void test_draw(void);
void update_partial_display(int x, int y, int new_x, int new_y, bounding_box* collision_box);
void update_asteroids();
void update_ship();

// Global variables
static int fbfd;
static uint16_t* fbmem;
static int error;
static struct fb_copyarea rect;
static uint16_t draw_color; // current color for drawing
static struct gamestate* my_gamestate;

// The screen transform structure is used to transform world coordinates to
// screen coordinates. The following computation is used to calculate screen
// coordinates from world coordinates:
//  1.    translate_x and translate_y is added to the world coordinates
//  2.    the world coordinates is divided by divide_x and divide_y
static struct screen_transform
{
  int translate_x;
  int translate_y;
} my_screen_transform;

void update_display(void)
{
  // update the display
  //rect.dx = 0;
  //rect.dy = 0;
  //rect.width  = DISPLAY_WIDTH;
  //rect.height = DISPLAY_HEIGHT;
  //ioctl(fbfd, 0x4680, &rect);

  // Update partial display
  update_ship();
  update_asteroids();
}

void update_asteroids() {
  asteroid** asteroids = my_gamestate->active_asteroids;
  asteroid* asteroid;

  for (int i = 0; i < my_gamestate->n_asteroids; ++i) {
    asteroid = asteroids[i];
    int old_x = asteroid->x_pos - asteroid->x_speed;
    int old_y = asteroid->y_pos - asteroid->y_speed;
    int new_x = asteroid->x_pos;
    int new_y = asteroid->y_pos;

    update_partial_display(old_x, old_y, new_x, new_y, &asteroid->collision_box);
  }
}

void update_ship() {
  ship_object* ship = &my_gamestate->ship;
  int old_x = ship->x_pos - ship->x_speed;
  int old_y = ship->y_pos - ship->y_speed;
  int new_x = ship->x_pos;
  int new_y = ship->y_pos;

  update_partial_display(old_x, old_y, new_x, new_y, &ship->collision_box);
}

void update_partial_display(int old_x, int old_y, int new_x, int new_y, bounding_box* collision_box) {
  // Convert to pixels coordinates
  old_x /= SCREEN_TO_WORLD_RATIO;
  old_y /= SCREEN_TO_WORLD_RATIO;
  new_x /= SCREEN_TO_WORLD_RATIO;
  new_y /= SCREEN_TO_WORLD_RATIO;
  int dx = abs(old_x - new_x) / SCREEN_TO_WORLD_RATIO;
  int dy = abs(old_y - new_y) / SCREEN_TO_WORLD_RATIO;
  int width = (collision_box->x_right_lower - collision_box->x_left_upper) / SCREEN_TO_WORLD_RATIO;
  int height = (collision_box->y_right_lower - collision_box->y_left_upper) / SCREEN_TO_WORLD_RATIO;

  // Determine if it is faster to update one or two rectangles
  if ((width + dx) * (height + dy) > 2 * width * height) {
    // Update one rectangle
    rect.dx = old_x;
    rect.dy = old_y;
    rect.width  = width;
    rect.height = height;

  } else {
    // Update two rectangles
    rect.dx = old_x;
    rect.dy = old_y;
    rect.width  = width;
    rect.height = height;
    // Update first of two rectangles
    ioctl(fbfd, 0x4680, &rect);

    // Only the x and y position has changed for the second rectangle
    rect.dx = new_x;
    rect.dy = new_y;
  }

  ioctl(fbfd, 0x4680, &rect);
}

void draw_all(void)
{
  draw_color = FB_COLOR_GRAY;
  do_draw_all();
}

void clear_all(void)
{
  draw_color = FB_COLOR_BLACK;
  do_draw_all();
}

void do_draw_all(void)
{
  const int num_asteroids = my_gamestate->n_asteroids;
  struct asteroid** asteroids = my_gamestate->active_asteroids;
  struct asteroid* asteroid;
  struct polygon* pol;
  struct ship_object* spaceship;

  
  // Draw all asteroids
  for(int i=0; i<num_asteroids; i++){
    asteroid = asteroids[i];
    my_screen_transform.translate_x = asteroid->x_pos;
    my_screen_transform.translate_y = asteroid->y_pos;
    pol = &(asteroid->poly);
    do_draw_polyline(pol->x_coords, pol->y_coords, pol->n_vertices);
  } 
  


  // Draw spaceship
  spaceship = &(my_gamestate->ship);
  my_screen_transform.translate_x = spaceship->x_pos;
  my_screen_transform.translate_y = spaceship->y_pos;
  pol = &(spaceship->poly);
  do_draw_polyline(pol->x_coords, pol->y_coords, pol->n_vertices);

  // TODO: Draw bullets
}

static void inline set_pixel(int x, int y)
{
  fbmem[x + y * DISPLAY_WIDTH] = FB_COLOR_GRAY;
}

static int inline get_index(int x, int y)
{
  return x + y * DISPLAY_WIDTH; 
}

void do_draw_polyline(int32_t* x_coords, int32_t* y_coords, int n_points)
{
  int x1 = (x_coords[0] + my_screen_transform.translate_x) / SCREEN_TO_WORLD_RATIO;
  int y1 = (y_coords[0] + my_screen_transform.translate_y) / SCREEN_TO_WORLD_RATIO;
  int x2;
  int y2;

  // Draw the polyline edges
  for(int i=1; i<n_points; i++){
    x2 = (x_coords[i] + my_screen_transform.translate_x) / SCREEN_TO_WORLD_RATIO;
    y2 = (y_coords[i] + my_screen_transform.translate_y) / SCREEN_TO_WORLD_RATIO;
    do_draw_line(x1, y1, x2, y2);
    x1 = x2;
    y1 = y2;
  }

  // Draw a line from the last to the first point
  x2 = (x_coords[0] + my_screen_transform.translate_x) / SCREEN_TO_WORLD_RATIO;
  y2 = (y_coords[0] + my_screen_transform.translate_y) / SCREEN_TO_WORLD_RATIO;
  do_draw_line(x1, y1, x2, y2);
}

// This function calls one of four line rasterization functions based on which
// octant the specified line resides in. Each function implements a version of
// the Bresenham line rasterization algorithm.
//
// NOTE: The algorithm was borrowed and adapted from:
//     Graphics & Visualization, Principles and Algorithms
// by:
//     T. Theoharis, G. Papaioannou, N. Platis, N. Patrikalakis
#define OCTANT_1 0b0000
#define OCTANT_2 0b0001
#define OCTANT_3 0b0011
#define OCTANT_4 0b0111
#define OCTANT_5 0b1111
#define OCTANT_6 0b1110
#define OCTANT_7 0b1100
#define OCTANT_8 0b1000
void do_draw_line(int sx, int sy, int ex, int ey)
{
  int dx = ex - sx;
  int dy = ey - sy;
  int octant_code = 0;
  int i_begin, i_end;

  // identify the octant of the point (dx, dy)
  octant_code |= (dx - dy < 0) << 0;
  octant_code |= (dx < 0)      << 1; 
  octant_code |= (dx + dy < 0) << 2;
  octant_code |= (dy < 0)      << 3;
  
  // depending on the octant, draw the line
  switch(octant_code){
    case OCTANT_8:
      i_begin = get_index(sx, sy);
      i_end   = get_index(ex, ey);
      draw_line_octant8(i_begin, i_end, dx, dy);
      break;
    case OCTANT_1:
      i_begin = get_index(sx, sy);
      i_end   = get_index(ex, ey);
      draw_line_octant1(i_begin, i_end, dx, dy);
      break;
    case OCTANT_2:
      i_begin = get_index(sx, sy);
      i_end   = get_index(ex, ey);
      draw_line_octant2(i_begin, i_end, dx, dy);
      break;
    case OCTANT_3:
      i_begin = get_index(sx, sy);
      i_end   = get_index(ex, ey);
      draw_line_octant3(i_begin, i_end, dx, dy);
      break;
    case OCTANT_4:
      i_begin = get_index(ex, ey);
      i_end   = get_index(sx, sy);
      draw_line_octant8(i_begin, i_end, -dx, -dy);
      break;
    case OCTANT_5:
      i_begin = get_index(ex, ey);
      i_end   = get_index(sx, sy);
      draw_line_octant1(i_begin, i_end, -dx, -dy);
      break;
    case OCTANT_6:
      i_begin = get_index(ex, ey);
      i_end   = get_index(sx, sy);
      draw_line_octant2(i_begin, i_end, -dx, -dy);
      break;
    case OCTANT_7:
      i_begin = get_index(ex, ey);
      i_end   = get_index(sx, sy);
      draw_line_octant3(i_begin, i_end, -dx, -dy);
      break;
    default:
      game_error("Illegal octant code: %d\n", octant_code);
  }
}

void draw_line_octant1(int i, int i_end, int dx, int dy)
{
  // Leap decision variable keeps track of when to increment y
  int e = -(dx >> 1);

  // Draw first pixel
  fbmem[i] = draw_color;

  // Draw remaining pixels
  while(i != i_end){
    
    // Update indexes and draw pixel
    i += 1;
    e += dy;
    if(e > 0){
      i += DISPLAY_WIDTH;
      e -= dx;
    }
    fbmem[i] = draw_color;
  }
}

void draw_line_octant2(int i, int i_end, int dx, int dy)
{
  // Leap decision variable keeps track of when to increment y.
  int e = -(dy >> 1);

  // Draw first pixel
  fbmem[i] = draw_color;

  // Draw remaining pixels
  while(i != i_end){
    
    // Update indexes and draw pixel
    i += DISPLAY_WIDTH;
    e += dx;
    if(e > 0){
      i += 1;
      e -= dy;
    }
    fbmem[i] = draw_color;
  }
}

void draw_line_octant3(int i, int i_end, int dx, int dy)
{
  // Leap decision variable keeps track of when to increment y.
  int e = -(dy >> 1);
  dx = -dx;

  // Draw first pixel
  fbmem[i] = draw_color;

  // Draw remaining pixels
  while(i != i_end){
    
    // Update indexes and draw pixel
    i += DISPLAY_WIDTH;
    e += dx;
    if(e > 0){
      i -= 1;
      e -= dy;
    }
    fbmem[i] = draw_color;
  }
}

void draw_line_octant8(int i, int i_end, int dx, int dy)
{
  // Leap decision variable keeps track of when to increment y.
  int e = -(dx >> 1);
  dy = -dy;

  // Draw first pixel
  fbmem[i] = draw_color;

  // Draw remaining pixels
  while(i != i_end){
    
    // Update indexes and draw pixel
    i += 1;
    e += dy;
    if(e > 0){
      i -= DISPLAY_WIDTH;
      e -= dx;
    }
    fbmem[i] = draw_color;
  }
}

void test_draw()
{
  game_debug("Running the drawing test ...\n");

  // a rectangle
  int rx = DISPLAY_WIDTH  / 16;
  int ry = DISPLAY_HEIGHT / 16;
  int rw = DISPLAY_WIDTH  - (2 * rx);
  int rh = DISPLAY_HEIGHT - (2 * ry);

  // center of the rectangle
  int cx = rx + (rw / 2);
  int cy = ry + (rh / 2);

  // set drawing color
  draw_color = FB_COLOR_GREEN;

  // draw the rectangle
  do_draw_line(rx         , ry         , rx + rw - 1, ry         );
  do_draw_line(rx + rw - 1, ry         , rx + rw - 1, ry + rh - 1);
  do_draw_line(rx + rw - 1, ry + rh - 1, rx         , ry + rh - 1);
  do_draw_line(rx         , ry + rh - 1, rx         , ry         );

  // draw lines from the center of the rectangle to the edges
  int lines = 10;
  for(int i=0; i<lines; i++){
    do_draw_line(cx, cy, rx + (rw * i / lines), ry);
    do_draw_line(cx, cy, rx + (rw * i / lines), ry + rh - 1);
    do_draw_line(cx, cy, rx, ry + (rh * i / lines));
    do_draw_line(cx, cy, rx + rw - 1, ry + (rh * i / lines));
  }
  game_debug("OK: Wrote lines to the framebuffer\n");

  // update the display
  rect.dx = 0;
  rect.dy = 0;
  rect.width  = DISPLAY_WIDTH;
  rect.height = DISPLAY_HEIGHT;
  ioctl(fbfd, 0x4680, &rect);
  game_debug("OK: Updated the display\n");

  game_debug("DONE: Drawing test completed. Can you see lines on the display?\n");
}

int init_draw(struct gamestate* gamestate)
{
  game_debug("Initializing draw module ...\n");

  // Remember the gamestate
  my_gamestate = gamestate;

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
      NULL,                               // let kernel decide physical memory address
      DISPLAY_SIZE * sizeof(uint16_t), // framebuffer size
      PROT_READ | PROT_WRITE,             // memory is read-write
      MAP_SHARED,                         // updates are carried immediately
      fbfd,                               // the mapped file
      0);                                 // offset
  if(fbmem == MAP_FAILED){
    game_error("Error mapping file to memory: %s\n", strerror(errno));
    return -1;
  }
  game_debug("OK: Mapped framebuffer device to memory\n");

  // Initialize the screen transform
  my_screen_transform.translate_x = 0;
  my_screen_transform.translate_y = 0;

  // Clear the display
  for(int i=0; i<DISPLAY_SIZE; i++){
    fbmem[i] = FB_COLOR_BLACK;
  }
  update_display();
  game_debug("OK: Removed Tux (some people have spheniscidaeaphobia)\n");

  // No errors
  game_debug("DONE: No errors initializing the draw module\n");
  return 0;
}

int release_draw()
{
  game_debug("Releasing draw module ...\n");

  // Unmap framebuffer device from memory
  error = munmap((void*)fbmem, DISPLAY_SIZE * sizeof(uint16_t));
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
  game_debug("DONE: No errors releasing module\n");
  return 0;
}
