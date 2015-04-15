#include "logic.h"
#include "input.h"

#define SHOOT         (1 << 0)
#define ROTATE_LEFT   (1 << 1)
#define ACCELERATE    (1 << 2)
#define ROTATE_RIGHT  (1 << 3)
#define MAX_AMOUNT_ASTEROIDS 40

void init_gamestate(void);
void do_logic(struct input);
struct ship init_ship(void);
struct asteroid* init_asteroid(void);
struct gamestate;
struct ship_object;
struct input;

static gamestate game;

struct input{
    uint8_t shoot : 1;
    uint8_t turn_left : 1;
    uint8_t accelerate : 1;
    uint8_t turn_right : 1;
};

// Implementation of the logic module
struct gamestate{
    struct ship_object ship;
    uint16_t n_asteroids;
    asteroid** asteroids;
}

// Struct for ship properties. Only one(two?) should ever exist.
struct ship_object{
    uint16_t acceleration;
    uint16_t orientation; // Could be float?
    int16_t x_speed;
    int16_t y_speed;
    uint16_t x_pos;
    uint16_t y_pos;
}

// Struct for asteroid properties
struct asteroid{
    int16_t x_speed;
    int16_t y_speed;
    uint16_t x_pos;
    uint16_t y_pos;
}

void init_gamestate(gamestate* game){
    new_gamestate = struct gamestate;
    new_gamestate.ship = make_ship();
    new_gamestate.asteroids = malloc(sizeOf(asteroid*)*MAX_AMOUNT_ASTEROIDS);
    return new_gamestate;
}

struct ship make_ship(void){
    struct new_ship = struct ship;
    new_ship.
    return new_ship;
}

// Allocate and return an asteroid object
//struct asteroid* make_asteroid(int16_t x_speed, int16_t y_speed){
struct asteroid* make_asteroid(void){ 
    asteroid* = malloc(sizeOf(asteroid));
    //asteroid->x_speed = x_speed;
    //asteroid->y_speed = y_speed;
    asteroid->x_speed = 10;
    asteroid->y_speed = 10;
    asteroid->x_pos = 100;
    asteroid->y_pos = 100;
    
    return asteroid;
}

void update_trajectory(struct input input){
    if(input.turn_left){
        ship.orientation++;
    }
    else if(input.turn_right){
        ship.orientation--;
    }
    if(input.accelerate){
        // TODO: Update speeds
    }
}

void update_gamestate(){
    ship.x_pos += ship.x_speed;
    ship.y_pos += ship.y_speed;
    
    for(int i = 0; i < gamestate.n_asteroids; i++){
        asteroid[i]->x_pos += asteroid[i]->x_speed;
        asteroid[i]->y_pos += asteroid[i]->y_speed;
    }
}
