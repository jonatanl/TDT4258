// Include guard
#ifndef _LOGIC_H
#define _LOGIC_H

#include <stdint.h>

// The world dimensions
#define SCREEN_TO_WORLD_RATIO (1 << 20)
#define DEFAULT_WORLD_X_DIM (DISPLAY_WIDTH  * SCREEN_TO_WORLD_RATIO)
#define DEFAULT_WORLD_Y_DIM (DISPLAY_HEIGHT * SCREEN_TO_WORLD_RATIO)

#define FRAMES_PER_SECOND 30




// All coordinates are relative to the logical game dimensions 
// rather than actual screen size
//
// NOTE: Polygon vertices must be listed in a counter-clockwise order in order
// for time_of_collision() to work.
struct polygon{
    int n_vertices;
    int32_t* x_coords;
    int32_t* y_coords;
};

// Bounding box of a shape, usually a polygon
struct bounding_box{
    int32_t x_left_upper;   // x_min
    int32_t y_left_upper;   // y_max
    int32_t x_right_lower;  // x_max
    int32_t y_right_lower;  // y_min
};

// Struct for ship properties. Only one(two?) should ever exist.
struct ship_object{
    int32_t x_pos;
    int32_t y_pos;
    int32_t x_speed;
    int32_t y_speed;
    float x_orientation;
    float y_orientation;
    int gun_cooldown;
    struct polygon poly;
    struct bounding_box collision_box; // Ship collision box and draw box
};

// Struct for asteroid properties
struct asteroid{
    uint8_t id;
    int32_t x_pos;
    int32_t y_pos;
    int32_t x_speed;
    int32_t y_speed;
    struct polygon poly;
    uint8_t type;   // small, med and big
    uint8_t index;  // when an asteroid is hit we need to know which
    struct bounding_box collision_box;  // Asteroid collision box

    // The draw box list. This is sent to the draw module to update the part of
    // the display covered by the asteroids movement.
    struct bounding_box* draw_boxes;
    int n_draw_boxes;
};

// Implementation of the logic module
struct gamestate{
    struct ship_object ship;
    struct asteroid* asteroids;             // A pointer to the list of asteroid structs in the segment
    int n_asteroids;                        // The counter for active asteroids
    int n_big_asteroids;    
    int n_med_asteroids;
    int n_sml_asteroids;
    struct asteroid** active_asteroids;     // A list of pointers to the my_asteroids list stored in the data segment
    int n_projectiles;
    struct projectile* projectiles;
    int32_t world_x_dim;
    int32_t world_y_dim; 
};

// Lacks a polygon at the moment    
struct projectile{
    int32_t x_pos;
    int32_t y_pos;
    int32_t x_speed;
    int32_t y_speed;
};

typedef struct ship_object ship_object;
typedef struct gamestate gamestate;
typedef struct polygon polygon;
typedef struct asteroid asteroid;
typedef struct projectile projectile;
typedef struct bounding_box bounding_box;

// Initialize and release module
int init_logic(struct gamestate** gamestate_ptr);
int release_logic();

// Updates the state of the game
// void update_gamestate();
void do_logic();
void do_logic_input(uint8_t input);

#endif // !_LOGIC_H
