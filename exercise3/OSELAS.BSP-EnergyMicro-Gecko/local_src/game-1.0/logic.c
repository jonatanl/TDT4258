// #include "input.h"

#include <stdlib.h>
#include <stdint.h>
#include "util.h"
#include "logic.h"

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

    update_ship(&(game.ship), input);
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

void update_ship(ship_object* ship, uint8_t input){
    if(CHECK_LEFT(input)){
        ship->orientation++;
    }
    else if(CHECK_RIGHT(input)){
        ship->orientation--;
    }
    if(CHECK_ACC(input)){
        // TODO: Update speeds
    }
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
    new_asteroid->real_poly.x_vertices = x_vertices;
    new_asteroid->real_poly.y_vertices = y_vertices;

    return new_asteroid;
}

ship_object make_ship(void){
    ship_object new_ship;
    new_ship.x_speed = 0;
    new_ship.y_speed = 0;
    new_ship.x_pos = 100;
    new_ship.y_pos = 100;
    new_ship.orientation = 0;

    // these values are probably pretty bad
    // TODO rethink this
    new_ship.real_poly.n_vertices = 3;
    uint16_t x_vertices[] = {0, 5, 10};
    uint16_t y_vertices[] = {0, 10, 0};
    new_ship.real_poly.x_vertices = x_vertices;
    new_ship.real_poly.y_vertices = y_vertices;

    return new_ship;
}

// Initializes the game struct
void init_logic(uint16_t n_asteroids, gamestate* game){
    game->ship = make_ship();
    game->asteroids = malloc(sizeof(asteroid*));
    for(int i = 0; i < n_asteroids; i++){

        // TODO rethink this
        uint16_t x_vertices[] = {0, 5, 10};
        uint16_t y_vertices[] = {0, 10, 0};
        game->asteroids[i] = make_asteroid(3, x_vertices, y_vertices);
    }
}
