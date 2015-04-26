// Include guard
#ifndef _LOGIC_H
#define _LOGIC_H

#include <stdint.h>

#include "game.h"

// This header collects all game structures in one place.
//
// NOTE: All coordinates are relative to world coordinates (the logical game
// dimensions) rather than screen coordinates.


// The world dimensions are calculated by multiplying the screen size with
// SCREEN_TO_WORLD_RATIO.
#define SCREEN_TO_WORLD_RATIO (1 << 20)
#define DEFAULT_WORLD_X_DIM (DISPLAY_WIDTH  * SCREEN_TO_WORLD_RATIO)
#define DEFAULT_WORLD_Y_DIM (DISPLAY_HEIGHT * SCREEN_TO_WORLD_RATIO)


// Number of game updates per second
#define FRAMES_PER_SECOND 30

// Type definitions
typedef struct polygon polygon;
typedef struct bounding_box bounding_box;
typedef struct spaceship spaceship;
typedef struct asteroid asteroid;
typedef struct projectile projectile;
typedef struct gamestate gamestate;

struct polygon{
    int n_vertices;
    int32_t* x_coords;
    int32_t* y_coords;
    // NOTE: Polygon vertices must be listed in a counter-clockwise order in
    // for get_intersection_time_poly_poly() and firends to work.
};

// Bounding box of a shape, usually a polygon
struct bounding_box{
    int32_t x_left_upper;   // x_min
    int32_t y_left_upper;   // y_max
    int32_t x_right_lower;  // x_max
    int32_t y_right_lower;  // y_min
};

struct spaceship{
    int32_t x_pos;
    int32_t y_pos;
    int32_t x_speed;
    int32_t y_speed;
    float x_orientation;
    float y_orientation;
    struct polygon poly;
    struct bounding_box collision_box;
    int gun_cooldown;
};

struct asteroid{
    int32_t x_pos;
    int32_t y_pos;
    int32_t x_speed;
    int32_t y_speed;
    struct polygon poly;
    struct bounding_box collision_box;
    uint8_t type;   // small, med and big
    uint8_t index;  // when an asteroid is hit we need to know which
    int32_t id;
};

struct projectile{
    int32_t x_pos;
    int32_t y_pos;
    int32_t x_speed;
    int32_t y_speed;
    struct polygon poly;
    struct bounding_box collision_box;
    int32_t lifetime;
    int32_t id; 
};

// Implementation of the logic module
struct gamestate{
    struct spaceship* ship;
    int n_asteroids;                        // The counter for active asteroids
    int n_big_asteroids;    
    int n_med_asteroids;
    int n_sml_asteroids;
    struct asteroid** active_asteroids;     // A list of pointers to the my_asteroids list stored in the data segment
    int n_projectiles;
    struct projectile** active_projectiles;
    int time;
};

// Initialize the game logic
int init_logic(struct gamestate** gamestate_ptr);

// Release game logic resources
int release_logic();

// Updates the state of the game logic
void update_logic();

#endif // !_LOGIC_H
