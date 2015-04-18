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

struct gamestate game;

void do_logic(uint8_t input){

    update_ship(&(game.ship));
    update_gamestate(&game);
    // Check collisions
}

void update_gamestate(gamestate* game){
    game->ship.x_pos += game->ship.x_speed;
    game->ship.y_pos += game->ship.y_speed;

    for(int i = 0; i < game->n_asteroids; i++){
        game->asteroids[i]->x_pos += game->asteroids[i]->x_speed;
        game->asteroids[i]->y_pos += game->asteroids[i]->y_speed;
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
asteroid* make_asteroid(uint16_t n_vertices, uint16_t* x_vertices, uint16_t* y_vertices){ 
    asteroid* new_asteroid = malloc(sizeof(asteroid));
    
    new_asteroid->x_speed = 10;
    new_asteroid->y_speed = 10;
    new_asteroid->x_pos = 100;
    new_asteroid->y_pos = 100;
    
    // Initialize the asteroids polygon
    new_asteroid->real_poly.n_vertices = n_vertices;
    new_asteroid->real_poly.x_coords = x_vertices;
    new_asteroid->real_poly.y_coords = y_vertices;

    return new_asteroid;
}

void init_ship(ship_object* ship){
    ship->x_speed = 0;
    ship->y_speed = 0;
    ship->x_pos = 100;
    ship->y_pos = 100;
    ship->x_orientation = 0;
    ship->y_orientation = 0;
    ship->gun_cooldown = 0;

    // these values are probably pretty bad
    // TODO rethink this
    ship->real_poly.n_vertices = 3;
    uint16_t* x_coords = malloc(sizeof(uint16_t)*3);
    uint16_t* y_coords = malloc(sizeof(uint16_t)*3);
    
    x_coords[0] = 0;
    x_coords[1] = 5;
    x_coords[2] = 10;

    y_coords[0] = 0;
    y_coords[1] = 10;
    y_coords[2] = 0;

    ship->real_poly.x_coords = x_coords;
    ship->real_poly.y_coords = y_coords;
}

// Initializes the game struct
void init_logic(uint16_t n_asteroids, gamestate* game){
    init_ship(&game->ship);
    game->asteroids = malloc(sizeof(asteroid*)*MAX_AMOUNT_ASTEROIDS);
    game->n_asteroids = 0;
    game->projectiles = malloc(sizeof(projectile*)*MAX_AMOUNT_PROJECTILES);
    game->n_projectiles = 0;
    game->world_x_dim = DEFAULT_WORLD_X_DIM;  // TODO set
    game->world_y_dim = DEFAULT_WORLD_Y_DIM;  // TODO set
    for(int i = 0; i < n_asteroids; i++){
        
        // TODO rethink this
        uint16_t* x_coords = malloc(sizeof(uint16_t)*3);
        uint16_t* y_coords = malloc(sizeof(uint16_t)*3);
        
        x_coords[0] = 0;
        x_coords[1] = 5;
        x_coords[2] = 10;

        y_coords[0] = 0;
        y_coords[1] = 10;
        y_coords[2] = 0;

        game->asteroids[i] = make_asteroid(3, x_coords, y_coords);
    }
}
