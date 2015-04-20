// Include guard
#ifndef _LOGIC_H
#define _LOGIC_H

#include <stdint.h>

// This module implements floating point values as 32-bit signed integers. The
// lower 20 bits specify the floating point part..
typedef int32_t ifloat;
#define FLOATING_DIGITS 20  // NOTE: Must be even!

// Functions that perform coordinate arithmetic
#define add(f1, f2)         (f1 + f2)
#define subtract(f1, f2)    (f1 + f2)
#define divide(f1, f2)      (f1 / f2)
#define multiply(f1, f2)    ((f1 >> (FLOATING_DIGITS / 2)) * (f2 >> (FLOATING_DIGITS / 2)))
#define int_to_ifloat(n)    (n << FLOATING_DIGITS)
#define float_to_ifloat(f)  (((int32_t)(f * (1 << FLOATING_DIGITS))) >> FLOATING_DIGITS)
#define ifloat_to_int(f)    (f >> FLOATING_DIGITS)

// Ratio to translate world coordinates to screen coordinates 
#define SCREEN_TO_FLOAT_RATIO   (1 << FLOATING_DIGITS)

// World size in ifloats
#define DEFAULT_WORLD_X_DIM     (320 * SCREEN_TO_FLOAT_RATIO)
#define DEFAULT_WORLD_Y_DIM     (240 * SCREEN_TO_FLOAT_RATIO)

#define FRAMES_PER_SECOND       30

// All coordinates are relative to the logical game dimensions 
// rather than actual screen size
struct polygon{
    int n_vertices;
    ifloat* x_coords;
    ifloat* y_coords;
    ifloat x_left_upper;
    ifloat y_left_upper;
    ifloat x_right_lower;
    ifloat y_right_lower;
};

// Struct for ship properties. Only one(two?) should ever exist.
struct ship_object{
    ifloat x_pos;
    ifloat y_pos;
    ifloat x_speed;
    ifloat y_speed;
    ifloat x_orientation;
    ifloat y_orientation;
    int gun_cooldown;
    struct polygon poly;
    // struct polygon current_poly;
};

// Struct for asteroid properties
struct asteroid{
    ifloat x_pos;
    ifloat y_pos;
    ifloat x_speed;
    ifloat y_speed;
    struct polygon poly;
}; 

// Implementation of the logic module
struct gamestate{
    struct ship_object ship;
    int n_asteroids;
    struct asteroid* asteroids;
    int n_projectiles;
    struct projectile* projectiles;
    ifloat world_x_dim;
    ifloat world_y_dim; 
};

// Lacks a polygon at the moment    
struct projectile{
    ifloat x_pos;
    ifloat y_pos;
    ifloat x_speed;
    ifloat y_speed;
};

typedef struct ship_object ship_object;
typedef struct gamestate gamestate;
typedef struct polygon polygon;
typedef struct asteroid asteroid;
typedef struct projectile projectile;

// Initialize and release module
int init_logic(struct gamestate** gamestate_ptr);
int release_logic();

// Updates the state of the game
// void update_gamestate();
void do_logic();

#endif // !_LOGIC_H
