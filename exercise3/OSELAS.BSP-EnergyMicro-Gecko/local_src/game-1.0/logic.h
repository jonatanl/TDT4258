// Include guard
#ifndef _LOGIC_H
#define _LOGIC_H

#include <stdint.h>

#define MAX_AMOUNT_ASTEROIDS    40
#define MAX_AMOUNT_PROJECTILES  10

// World size
#define DEFAULT_WORLD_X_DIM     (20 << 25)
#define DEFAULT_WORLD_Y_DIM     (15 << 25)

// Constant to translate world coordinates to screen coordinates 
#define WORLD_TO_SCREEN_RATIO   (1  << 21)
#define FRAMES_PER_SECOND       30

// Used to calculate length of acceleration vector
#define SECONDS_TO_CROSS_SCREEN 3

#define FRAMES_TO_CROSS_SCREEN


// All coordinates are relative to the logical game dimensions 
// rather than actual screen size
struct polygon{
    uint16_t n_vertices;
    uint16_t* x_coords;
    uint16_t* y_coords;
};

// Struct for ship properties. Only one(two?) should ever exist.
struct ship_object{
    int16_t x_orientation;
    int16_t y_orientation;
    int16_t x_speed;
    int16_t y_speed;
    uint16_t x_pos;
    uint16_t y_pos;
    uint16_t gun_cooldown;
    struct polygon real_poly;
    // struct polygon current_poly;
};

// Struct for asteroid properties
struct asteroid{
    int16_t x_speed;
    int16_t y_speed;
    uint16_t x_pos;
    uint16_t y_pos;
    struct polygon real_poly;
}; 

// Implementation of the logic module
struct gamestate{
    struct ship_object ship;
    uint16_t n_asteroids;
    struct asteroid** asteroids;
    uint16_t n_projectiles;
    struct projctile** projectiles;
    int32_t world_x_dim;
    int32_t world_y_dim; 
};

// Lacks a polygon at the moment    
struct projectile{
    int16_t x_speed;
    int16_t y_speed;
    uint16_t x_pos;
    uint16_t y_pos;
};

typedef struct ship_object ship_object;
typedef struct gamestate gamestate;
typedef struct polygon polygon;
typedef struct asteroid asteroid;
typedef struct projectile projectile;


void do_logic(uint8_t input);
void update_gamestate(struct gamestate* game);
void init_ship(ship_object* ship);
struct asteroid* init_asteroid(void);
void rotate_polygon(struct polygon poly, int16_t degrees);
void update_ship(struct ship_object* ship);
void init_logic(uint16_t n_asteroids, struct gamestate* game);
asteroid* make_asteroid(uint16_t n_vertices, uint16_t* x_vertices, uint16_t* y_vertices);
void do_shoot(void);

#endif // !_LOGIC_H
