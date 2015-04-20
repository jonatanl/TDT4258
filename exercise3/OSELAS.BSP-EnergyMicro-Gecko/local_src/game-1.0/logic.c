#include <stdlib.h>
#include <stdint.h>

#include "util.h"
#include "logic.h"
#include "input.h"
#define DEBUG
#include "debug.h"

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
////////////////
////////////////        Game logic
////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

// Calculates the length of the acceleration vector
#define MILLISECONDS_TO_CROSS_SCREEN 3000
#define FRAMES_TO_CROSS_SCREEN ((FRAMES_PER_SECOND * MILLISECONDS_TO_CROSS_SCREEN) / 1000)
#define DEFAULT_ACCELERATION (DEFAULT_WORLD_X_DIM / (FRAMES_TO_CROSS_SCREEN * (FRAMES_TO_CROSS_SCREEN + 1)))

#define MAX_AMOUNT_ASTEROIDS    40
#define MAX_AMOUNT_PROJECTILES  10

// Function prototypes
void init_ship(struct ship_object* ship);
void init_asteroid(int n_coords, ifloat* x_coords, ifloat* y_coords, struct asteroid* asteroid);
void do_logic(uint8_t input);
void do_shoot(void);
void update_ship();

// Global variables
struct gamestate game;
struct asteroid my_asteroids[MAX_AMOUNT_ASTEROIDS];
struct projectile my_projectiles[MAX_AMOUNT_PROJECTILES];

void do_logic(uint8_t input){

    update_ship(&game.ship);
    update_gamestate(&game);
    // Check collisions
}

void update_gamestate(){
    game.ship.x_pos = add(game.ship.x_pos, game.ship.x_speed);
    game.ship.y_pos = add(game.ship.y_pos, game.ship.y_speed);

    for(int i = 0; i < game.n_asteroids; i++){
        game.asteroids[i].x_pos += add(game.asteroids[i].x_pos, game.asteroids[i].x_speed);
        game.asteroids[i].y_pos += add(game.asteroids[i].y_pos, game.asteroids[i].y_speed);
    }
}

// Handles input
void update_ship(){
    uint8_t input = get_input();

    if(CHECK_PAUSE(input)){
        // Do pause
    }


    // If both left and right is pressed the ship does nothing
    // if else, check if turn, do roation, and normalize
    if(!(CHECK_LEFT(input) && CHECK_RIGHT(input))){   
        if(CHECK_LEFT(input)){
            // TODO rotation
        }
        else if(CHECK_RIGHT(input)){
            // TODO rotation
        }
    }
    if(CHECK_ACC(input)){
            // TODO orientation

        // TODO: Update speeds
    }
    // Decrements the gun cooldown, or checks if shoot is pressed and fires a shot
    if(game.ship.gun_cooldown){
        game.ship.gun_cooldown--;
    }
    else if(CHECK_SHOOT(input)){
        do_shoot();
    }
}


void do_shoot(void){

}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
////////////////
////////////////        Initializer methods
////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////


// Initialize an asteroid
void init_asteroid(int n_coords, ifloat* x_coords, ifloat* y_coords, struct asteroid* new_asteroid){ 
  
  // Set speed and position
  new_asteroid->x_pos = int_to_ifloat(100);
  new_asteroid->y_pos = int_to_ifloat(100);
  new_asteroid->x_speed = int_to_ifloat(10);
  new_asteroid->y_speed = int_to_ifloat(10);
  
  // Initialize the asteroid polygon
  new_asteroid->poly.n_vertices = n_coords;
  new_asteroid->poly.x_coords = x_coords;
  new_asteroid->poly.y_coords = y_coords;
}

// Initialize a spaceship
void init_ship(struct ship_object* ship){
    ship->x_speed = 0;
    ship->y_speed = 0;
    ship->x_pos = DEFAULT_WORLD_X_DIM / 2;
    ship->y_pos = DEFAULT_WORLD_Y_DIM / 2;
    ship->x_orientation = 0;
    ship->y_orientation = int_to_ifloat(1);
    ship->gun_cooldown = 0;

    // these values are probably pretty bad
    // TODO rethink this
    ship->poly.n_vertices = 3;
    ifloat* x_coords = malloc(sizeof(ifloat)*3);
    ifloat* y_coords = malloc(sizeof(ifloat)*3);
    
    x_coords[0] = add(ship->x_pos, int_to_ifloat(0));
    x_coords[1] = add(ship->x_pos, int_to_ifloat(5));
    x_coords[2] = add(ship->x_pos, int_to_ifloat(10));

    y_coords[0] = add(ship->y_pos, int_to_ifloat(0));
    y_coords[1] = add(ship->y_pos, int_to_ifloat(5));
    y_coords[2] = add(ship->y_pos, int_to_ifloat(10));

    ship->poly.x_coords = x_coords;
    ship->poly.y_coords = y_coords;
}

// Initializes the module
int init_logic(struct gamestate** gamestate_ptr){
  game_debug("Initializing the logic module ...\n");

  // Initialize the gamestate struct
  init_ship(&game.ship);
  game.asteroids = &my_asteroids[0];
  game.n_asteroids = 0;
  game.projectiles = &my_projectiles[0];
  game.n_projectiles = 0;
  game.world_x_dim = DEFAULT_WORLD_X_DIM;
  game.world_y_dim = DEFAULT_WORLD_Y_DIM;

  // Initialize all asteroids
  // TODO: rethink this
  for(int i = 0; i < MAX_AMOUNT_ASTEROIDS; i++){
      
      // Initialize polygon vertices
      ifloat* x_coords = malloc(sizeof(ifloat)*3);
      ifloat* y_coords = malloc(sizeof(ifloat)*3);
      x_coords[0] = int_to_ifloat(0);
      x_coords[1] = int_to_ifloat(5);
      x_coords[2] = int_to_ifloat(10);
      y_coords[0] = int_to_ifloat(0);
      y_coords[1] = int_to_ifloat(10);
      y_coords[2] = int_to_ifloat(0);

      init_asteroid(3, x_coords, y_coords, &game.asteroids[i]);
  }
  *gamestate_ptr = &game;

  // No errors
  game_debug("DONE: No errors initializing the logic module\n");
  return 0;
}

// Initializes the module
int release_logic(){
  game_debug("Releasing the logic module ...\n");

  // TODO: Relase module resources:
  //  - dynamically allocated polygons

  // No errors
  game_debug("DONE: No errors releasing the logic module\n");
  return 0;
}
