#include <fcntl.h>     // open()
#include <unistd.h>    // close()
#include <string.h>    // strerror()
#include <errno.h>     // errno
#include <sys/mman.h>  // mmap(), munmap()
#include <sys/ioctl.h> // ioctl()
#include <linux/fb.h>  // struct fb_copyarea
#include <stdlib.h>    // absolute value abs()

#include "draw.h"
#include "game.h"
// #define DEBUG
#include "debug.h"
#include "logic.h"
#include "util.h"
#include "geometry.h"

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
static void do_update_moving_bounding_box( int32_t x_pos_new, int32_t y_pos_new, int32_t x_speed, int32_t y_speed, bounding_box* draw_box);
static void do_update_partial_display(int x, int y, int new_x, int new_y, bounding_box collision_box);
static void do_update_rectangle(int32_t x_left_upper, int32_t y_left_upper, int32_t x_right_lower, int32_t y_right_lower);
static void do_draw_all(void);
static void do_draw_polyline(int32_t* x_coords, int32_t* y_coords, int n_points);
static void do_draw_line(int sx, int sy, int ex, int ey);
static void draw_line_octant1(int is, int ie, int dx, int dy);
static void draw_line_octant2(int is, int ie, int dx, int dy);
static void draw_line_octant3(int is, int ie, int dx, int dy);
static void draw_line_octant8(int is, int ie, int dx, int dy);
static void inline closest_screen_coordinate(int32_t* x, int32_t* y);
static void inline world_to_screen_coordinate(int32_t* x, int32_t* y);
static bool inline line_inside_screen(int32_t x1, int32_t y1, int32_t x2, int32_t y2);
static bool inline point_inside_screen(int32_t x, int32_t y);

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

void update_partial_display(void)
{
  asteroid** asteroids = my_gamestate->active_asteroids;
  asteroid* asteroid;
  struct projectile** projectiles = my_gamestate->active_projectiles;
  struct projectile* projectile;
  spaceship* spaceship = my_gamestate->ship;

  // Update asteroids
  for (int i = 0; i < my_gamestate->n_asteroids; ++i) {
    asteroid = asteroids[i];
    do_update_moving_bounding_box(
        asteroid->x_pos,
        asteroid->y_pos,
        asteroid->x_speed,
        asteroid->y_speed,
        &asteroid->collision_box
    );
  }
  
  // Update spaceship
  do_update_moving_bounding_box(
      spaceship->x_pos,
      spaceship->y_pos,
      spaceship->x_speed,
      spaceship->y_speed,
      &spaceship->collision_box
  );

  // Draw all projectiles
  for(int i=0; i < my_gamestate->n_projectiles; i++){
    projectile = projectiles[i];
    do_update_moving_bounding_box(
      projectile->x_pos,
      projectile->y_pos,
      projectile->x_speed,
      projectile->y_speed,
      &projectile->collision_box
    );
  }
}

void update_full_display(void){
  rect.dx = 0;
  rect.dy = 0;
  rect.width  = DISPLAY_WIDTH;
  rect.height = DISPLAY_HEIGHT;
  ioctl(fbfd, 0x4680, &rect);
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
  update_full_display();
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

void do_update_moving_asteroid(struct asteroid* ast){
  do_update_moving_bounding_box(
      ast->x_pos,
      ast->y_pos,
      ast->x_speed,
      ast->y_speed,
      &(ast->collision_box)
  );
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
////////////////
////////////////        Static functions
////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

// Updates a rectangular screen area that contains the collision box before and after movement
static void do_update_moving_bounding_box(
    int32_t x_pos_new, // the position of the bounding box after movement
    int32_t y_pos_new, // 
    int32_t x_speed, // the speed of the bounding box
    int32_t y_speed, //
    bounding_box* bounding_box)
{

  // Calculate bounding box world coordinates
  int32_t x_left_upper  = x_pos_new + bounding_box->x_left_upper;
  int32_t y_left_upper  = y_pos_new + bounding_box->y_left_upper;
  int32_t x_right_lower = x_pos_new + bounding_box->x_right_lower;
  int32_t y_right_lower = y_pos_new + bounding_box->y_right_lower;

  // Modify coordinates to account for movement
  if(x_speed >= 0){
    x_left_upper  -= x_speed;
  }else{
    x_right_lower -= x_speed;
  }
  if(y_speed >= 0){
    y_left_upper  -= y_speed;
  }else{
    y_right_lower -= y_speed;
  }

  // Convert world coordinates to screen coordinates
  x_left_upper = x_left_upper / SCREEN_TO_WORLD_RATIO - 2;      // round down
  y_left_upper = y_left_upper / SCREEN_TO_WORLD_RATIO - 2;      //
  x_right_lower = (x_right_lower + SCREEN_TO_WORLD_RATIO - 1) / SCREEN_TO_WORLD_RATIO + 2; // round up
  y_right_lower = (y_right_lower + SCREEN_TO_WORLD_RATIO - 1) / SCREEN_TO_WORLD_RATIO + 2; //

  // Update the rectangular area
  do_update_rectangle(x_left_upper, y_left_upper, x_right_lower, y_right_lower);
}

static void do_update_rectangle(int32_t x_left_upper, int32_t y_left_upper, int32_t x_right_lower, int32_t y_right_lower){

  // If the rectangle overlaps any vertical boundaries, split it in two and recurse
  if(x_left_upper < 0){
    do_update_rectangle(
        x_left_upper + DISPLAY_WIDTH,
        y_left_upper,
        DISPLAY_WIDTH - 1,
        y_right_lower
    );
    x_left_upper = 0;
  }else if(x_right_lower > DISPLAY_WIDTH - 1){
    do_update_rectangle(
        0,
        y_left_upper,
        x_right_lower - DISPLAY_WIDTH,
        y_right_lower
    );
    x_right_lower = DISPLAY_WIDTH - 1;
  }
    
  // If the rectangle overlaps any horizontal boundaries, split it in two and recurse
  if(y_left_upper < 0){
    do_update_rectangle(
        x_left_upper,
        y_left_upper + DISPLAY_HEIGHT,
        x_right_lower,
        DISPLAY_HEIGHT - 1
    );
    y_left_upper = 0;
  }else if(y_right_lower > DISPLAY_HEIGHT - 1){
    do_update_rectangle(
        x_left_upper,
        0,
        x_right_lower,
        y_right_lower - DISPLAY_HEIGHT
    );
    y_right_lower = DISPLAY_HEIGHT - 1;
  }

  // Set update rectangle and update screen
  rect.dx = x_left_upper;
  rect.dy = y_left_upper;
  rect.width  = x_right_lower - x_left_upper + 1;
  rect.height = y_right_lower - y_left_upper + 1;
  game_debug("rect: %d, %d, %d, %d\n", rect.dx, rect.dy, rect.width, rect.height);
  ioctl(fbfd, 0x4680, &rect);
}

// NOTE: NOT USED AND WITH BUGS. Use do_update_bounding_box() instead.
static void do_update_partial_display(int old_x, int old_y, int new_x, int new_y, bounding_box collision_box) {

  // Convert to screen coordinates and make shure they are inside the screen
  world_to_screen_coordinate(&old_x, &old_y);
  closest_screen_coordinate(&old_x, &old_y);

  // Convert to screen coordinates and make shure they are inside the screen
  world_to_screen_coordinate(&new_x, &new_y);
  closest_screen_coordinate(&new_x, &new_y);
  
  int dx = abs(old_x - new_x);
  int dy = abs(old_y - new_y);

  // Convert to screen coordinates and make shure they are inside the screen
  world_to_screen_coordinate(&collision_box.x_right_lower, &collision_box.y_right_lower);
  closest_screen_coordinate(&collision_box.y_right_lower, &collision_box.y_right_lower);

  // Convert to screen coordinates and make shure they are inside the screen
  world_to_screen_coordinate(&collision_box.x_left_upper, &collision_box.y_left_upper);
  closest_screen_coordinate(&collision_box.x_left_upper, &collision_box.y_left_upper);

  int width = collision_box.x_right_lower - collision_box.x_left_upper;
  int height = collision_box.y_right_lower - collision_box.y_left_upper;

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

static void do_draw_all(void)
{
  const int num_asteroids = my_gamestate->n_asteroids;
  const int num_projectiles = my_gamestate->n_projectiles;
  
  struct asteroid** asteroids = my_gamestate->active_asteroids;
  struct asteroid* asteroid;
  struct projectile** projectiles = my_gamestate->active_projectiles;
  struct projectile* projectile;
  struct spaceship* spaceship;
  struct polygon* pol;
  
  // Draw all asteroids
  for(int i=0; i<num_asteroids; i++){
    
    asteroid = asteroids[i];
    my_screen_transform.translate_x = asteroid->x_pos;
    my_screen_transform.translate_y = asteroid->y_pos;
    pol = &(asteroid->poly);
    do_draw_polyline(pol->x_coords, pol->y_coords, pol->n_vertices);
  } 

  if(projectiles == NULL){
    game_debug("NULL projectiles\n");
  }

  // Draw all projectiles
  for(int i=0; i<num_projectiles; i++){

    if(projectiles[i] == NULL){
      game_debug("NULL projectile[%d]\n", i);
    }

    projectile = projectiles[i];
    my_screen_transform.translate_x = projectile->x_pos;
    my_screen_transform.translate_y = projectile->y_pos;
    pol = &(projectile->poly);
    do_draw_polyline(pol->x_coords, pol->y_coords, pol->n_vertices);

  }

  // Draw spaceship
  spaceship = my_gamestate->ship;
  my_screen_transform.translate_x = spaceship->x_pos;
  my_screen_transform.translate_y = spaceship->y_pos;
  pol = &(spaceship->poly);
  do_draw_polyline(pol->x_coords, pol->y_coords, pol->n_vertices);
}

static void inline set_pixel(int x, int y)
{
  fbmem[x + y * DISPLAY_WIDTH] = FB_COLOR_GRAY;
}

static int inline get_index(int x, int y)
{
  return x + y * DISPLAY_WIDTH; 
}

static void inline world_to_screen_coordinate(int32_t* x, int32_t* y)
{
  *x /= SCREEN_TO_WORLD_RATIO;
  *y /= SCREEN_TO_WORLD_RATIO;
}

static void inline closest_screen_coordinate(int32_t* x, int32_t* y)
{
  *x = ARG_MIN(*x, DISPLAY_WIDTH - 1);
  *x = ARG_MAX(*x, 0);
  *y = ARG_MIN(*y, DISPLAY_HEIGHT - 1);
  *y = ARG_MAX(*y, 0);
}

// Returns true if the point (x, y) is inside the screen
static bool inline point_inside_screen(int32_t x, int32_t y)
{
  return (0 <= x) | (x < DISPLAY_WIDTH)
       | (0 <= y) | (y < DISPLAY_HEIGHT);
}

// Returns true if the line from (x1, y1) to (x2, y2) is inside the screen
static bool inline line_inside_screen(int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
  return (0 <= x1) | (x1 < DISPLAY_WIDTH)
       | (0 <= x2) | (x2 < DISPLAY_WIDTH)
       | (0 <= y1) | (y1 < DISPLAY_HEIGHT)
       | (0 <= y2) | (y2 < DISPLAY_HEIGHT);
}

static void do_draw_polyline(int32_t* x_coords, int32_t* y_coords, int n_points)
{
  int x1;
  int y1;
  int x2;
  int y2;
  int clipcode1;
  int clipcode2;

  x1 = (x_coords[0] + my_screen_transform.translate_x);
  y1 = (y_coords[0] + my_screen_transform.translate_y);
  world_to_screen_coordinate(&x1, &y1); 

  // Draw the polyline edges
  for(int i=1; i<n_points; i++){

    // Calculate the next line end-point
    x2 = (x_coords[i] + my_screen_transform.translate_x);
    y2 = (y_coords[i] + my_screen_transform.translate_y);
    world_to_screen_coordinate(&x2, &y2); 

    // Compute the clipping codes
    clipcode1 = 0;
    clipcode1 |= (x1 < 0)               << 0;
    clipcode1 |= (x1 >= DISPLAY_WIDTH)  << 1;
    clipcode1 |= (y1 < 0)               << 2;
    clipcode1 |= (y1 >= DISPLAY_HEIGHT) << 3;
    clipcode2 = 0;
    clipcode2 |= (x2 < 0)               << 0;
    clipcode2 |= (x2 >= DISPLAY_WIDTH)  << 1;
    clipcode2 |= (y2 < 0)               << 2;
    clipcode2 |= (y2 >= DISPLAY_HEIGHT) << 3;

    if((clipcode1 | clipcode2) == 0){       // line is entirely inside
      do_draw_line(x1, y1, x2, y2);
    }else if((clipcode1 & clipcode2) != 0){ // line is entirely outside
      // Do nothing
    }else {                                 // indeterminate
      // TODO: Check clipping code and split the line in two
    }

    // Reuse the calculated coordinates for the next line
    x1 = x2;
    y1 = y2;
  }

  // Draw a line from the last to the first point
  x2 = (x_coords[0] + my_screen_transform.translate_x) / SCREEN_TO_WORLD_RATIO;
  y2 = (y_coords[0] + my_screen_transform.translate_y) / SCREEN_TO_WORLD_RATIO;
  if(line_inside_screen(x1, y1, x2, y2)){
    do_draw_line(x1, y1, x2, y2);
  }
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
static void do_draw_line(int sx, int sy, int ex, int ey)
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

static void draw_line_octant1(int i, int i_end, int dx, int dy)
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

static void draw_line_octant2(int i, int i_end, int dx, int dy)
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

static void draw_line_octant3(int i, int i_end, int dx, int dy)
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

static void draw_line_octant8(int i, int i_end, int dx, int dy)
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
