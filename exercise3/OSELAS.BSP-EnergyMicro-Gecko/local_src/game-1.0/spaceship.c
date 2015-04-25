#include <stdbool.h>

#include "spaceship.h"
#include "input.h"
#include "logic.h"
#include "util.h"

// Prototypes
static void rotate_spaceship(bool clockwise);

// Global variables
struct spaceship my_spaceship;

// The spaceship acceleration factor as a constant expression
#define MILLISECONDS_TO_CROSS_SCREEN 3000
#define FRAMES_TO_CROSS_SCREEN ((FRAMES_PER_SECOND * MILLISECONDS_TO_CROSS_SCREEN) / 1000)
#define DEFAULT_ACCELERATION (DEFAULT_WORLD_X_DIM / (FRAMES_TO_CROSS_SCREEN * (FRAMES_TO_CROSS_SCREEN + 1)))

// Defines the ship polygon
#define DEFAULT_N_VERTICES 3
static int32_t my_x_coords[3] = {
     0 * SCREEN_TO_WORLD_RATIO,
     7 * SCREEN_TO_WORLD_RATIO,
    -7 * SCREEN_TO_WORLD_RATIO,
};
static int32_t my_y_coords[3] = {
     9 * SCREEN_TO_WORLD_RATIO,
    -6 * SCREEN_TO_WORLD_RATIO,
    -6 * SCREEN_TO_WORLD_RATIO,
};

void init_spaceship(struct spaceship** ship){

    // Initialize speed, position, orientation and gun_cooldown
    my_spaceship.x_speed = 0;
    my_spaceship.y_speed = 0;
    my_spaceship.x_pos = DEFAULT_WORLD_X_DIM / 2;
    my_spaceship.y_pos = DEFAULT_WORLD_Y_DIM / 2;
    my_spaceship.x_orientation =  0; // Default orientation is up
    my_spaceship.y_orientation = -1; //
    my_spaceship.gun_cooldown = 0; // Not used right now

    // Set spaceship polygon
    my_spaceship.poly.n_vertices = DEFAULT_N_VERTICES;
    my_spaceship.poly.x_coords = &my_x_coords[0];
    my_spaceship.poly.y_coords = &my_y_coords[0];

    // Return the spaceship
    *ship = &my_spaceship;
}

void release_spaceship(){
  // Do nothing
}

void update_spaceship(uint8_t input){

    // Update orientation
    //
    // If both left and right is pressed the ship does nothing
    // if else, check if turn, do roation, and normalize
    if(!(CHECK_LEFT(input) && CHECK_RIGHT(input))){   
        
        if(CHECK_LEFT(input)){
            rotate_spaceship(true);  // rotate counterclockwise
        }
        else if(CHECK_RIGHT(input)){
            rotate_spaceship(false); // rotate counterclockwise
        }
    }

    // Update speed 
    if(CHECK_ACC(input)){
        my_spaceship.x_speed += my_spaceship.x_orientation * DEFAULT_ACCELERATION;
        my_spaceship.y_speed += my_spaceship.y_orientation * DEFAULT_ACCELERATION;
    }

    // Update position
    my_spaceship.x_pos = my_spaceship.x_pos + my_spaceship.x_speed;
    my_spaceship.y_pos = my_spaceship.y_pos + my_spaceship.y_speed;

    // Update the ship bounding box
    create_bounding_box(
        &my_spaceship.collision_box,
        &my_spaceship.poly
    );

    // TODO: Wrap around with do_wrap(&game.ship.x_pos, &game.ship.y_pos);
}

static void rotate_spaceship(bool clockwise) {
	int numberOfVertices = my_spaceship.poly.n_vertices;

	for (int i = 0; i < numberOfVertices; ++i) {
		rotate_coordinate_int(&my_spaceship.poly.x_coords[i], &my_spaceship.poly.y_coords[i], 0, 0, clockwise);
	}
  rotate_coordinate_float(&my_spaceship.x_orientation, &my_spaceship.y_orientation, 0, 0, clockwise);
}
