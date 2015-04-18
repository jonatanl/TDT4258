// #include "input.h"

#include <stdlib.h>
#include <stdint.h>
#include "util.h"
#include "logic.h"
#include "input.h"

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
void do_logic(uint8_t input);
void update_ship(struct ship_object* ship);
void do_shoot(void);
asteroid* make_asteroid(int n_coords, ifloat* x_coords, ifloat* y_coords);
void init_ship(ship_object* ship);
struct asteroid* init_asteroid(void);

// Global variables
struct gamestate game;

void do_logic(uint8_t input){

    update_ship(&(game.ship));
    update_gamestate(&game);
    // Check collisions
}

void update_gamestate(gamestate* game){
    game->ship.x_pos = add(game->ship.x_pos, game->ship.x_speed);
    game->ship.y_pos = add(game->ship.y_pos, game->ship.y_speed);

    for(int i = 0; i < game->n_asteroids; i++){
        game->asteroids[i]->x_pos += add(game->asteroids[i]->x_pos, game->asteroids[i]->x_speed);
        game->asteroids[i]->y_pos += add(game->asteroids[i]->y_pos, game->asteroids[i]->y_speed);
    }
}

// Handles input
void update_ship(ship_object* ship){
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
    if(ship->gun_cooldown){
        ship->gun_cooldown--;
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


// Allocate and return an asteroid object
asteroid* make_asteroid(int n_coords, ifloat* x_coords, ifloat* y_coords){ 
    asteroid* new_asteroid = malloc(sizeof(asteroid));
    
    new_asteroid->x_speed = int_to_ifloat(10);
    new_asteroid->y_speed = int_to_ifloat(10);
    new_asteroid->x_pos = int_to_ifloat(100);
    new_asteroid->y_pos = int_to_ifloat(100);
    
    // Initialize the asteroids polygon
    new_asteroid->real_poly.n_vertices = n_coords;
    new_asteroid->real_poly.x_coords = x_coords;
    new_asteroid->real_poly.y_coords = y_coords;

    return new_asteroid;
}

void init_ship(ship_object* ship){
    ship->x_speed = 0;
    ship->y_speed = 0;
    ship->x_pos = DEFAULT_WORLD_X_DIM / 2;
    ship->y_pos = DEFAULT_WORLD_Y_DIM / 2;
    ship->x_orientation = 0;
    ship->y_orientation = int_to_ifloat(1);
    ship->gun_cooldown = 0;

    // these values are probably pretty bad
    // TODO rethink this
    ship->real_poly.n_vertices = 3;
    ifloat* x_coords = malloc(sizeof(ifloat)*3);
    ifloat* y_coords = malloc(sizeof(ifloat)*3);
    
    x_coords[0] = int_to_ifloat(0);
    x_coords[1] = int_to_ifloat(5);
    x_coords[2] = int_to_ifloat(10);

    y_coords[0] = int_to_ifloat(0);
    y_coords[1] = int_to_ifloat(10);
    y_coords[2] = int_to_ifloat(0);

    ship->real_poly.x_coords = x_coords;
    ship->real_poly.y_coords = y_coords;
}

// Initializes the game struct
gamestate* init_logic(){
    init_ship(&game.ship);
    game.asteroids = malloc(sizeof(asteroid*)*MAX_AMOUNT_ASTEROIDS);
    game.n_asteroids = 0;
    game.projectiles = malloc(sizeof(projectile*)*MAX_AMOUNT_PROJECTILES);
    game.n_projectiles = 0;
    game.world_x_dim = DEFAULT_WORLD_X_DIM;  // TODO set
    game.world_y_dim = DEFAULT_WORLD_Y_DIM;  // TODO set
    for(int i = 0; i < n_asteroids; i++){
        
        // TODO rethink this
        ifloat* x_coords = malloc(sizeof(ifloat)*3);
        ifloat* y_coords = malloc(sizeof(ifloat)*3);
        
        x_coords[0] = int_to_ifloat(0);
        x_coords[1] = int_to_ifloat(5);
        x_coords[2] = int_to_ifloat(10);

        y_coords[0] = int_to_ifloat(0);
        y_coords[1] = int_to_ifloat(10);
        y_coords[2] = int_to_ifloat(0);

        game.asteroids[i] = make_asteroid(3, x_coords, y_coords);
    }
    return &game;
}
