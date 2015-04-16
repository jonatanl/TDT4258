#include "logic.h"
#include "input.h"

#define SHOOT         (1 << 0)
#define ROTATE_LEFT   (1 << 1)
#define ACCELERATE    (1 << 2)
#define ROTATE_RIGHT  (1 << 3)
#define MAX_AMOUNT_ASTEROIDS 40

struct gamestate;
struct ship_object;
struct asteroid;
struct vertice;
struct polygon;
struct input;
void do_logic(struct input);
struct ship_object init_ship(void);
struct asteroid* init_asteroid(void);
void rotate_polygon(struct polygon poly, int16_t degrees);
void update_ship(struct ship_object* ship, struct input input);
void init_logic(uint16_t n_asteroids, struct gamestate* game);


///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
////////////////
////////////////        Object definitions
////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

// The concept of a polygon is vital both for collision detection and rendering, so they are updated here.
struct vertice{
    uint16_t x_pos;
    uint16_t y_pos;
};

struct polygon{
    uint16_t n_vertices;
    struct vertice* vertices;
};

struct input{
    uint8_t shoot : 1;
    uint8_t turn_left : 1;
    uint8_t accelerate : 1;
    uint8_t turn_right : 1;
};

// Struct for ship properties. Only one(two?) should ever exist.
struct ship_object{
    uint16_t orientation; // Could be float? Currently should be something like 0 - 360
    int16_t x_speed;
    int16_t y_speed;
    uint16_t x_pos;
    uint16_t y_pos;
    struct polygon poly;
};

// Struct for asteroid properties
struct asteroid{
    int16_t x_speed;
    int16_t y_speed;
    uint16_t x_pos;
    uint16_t y_pos;
    struct polygon poly;
}; 

// Implementation of the logic module
struct gamestate{
    struct ship_object ship;
    uint16_t n_asteroids;
    struct asteroid** asteroids;
};

typedef struct vertice vertice;
typedef struct ship_object ship_object;
typedef struct gamestate gamestate;
typedef struct polygon polygon;
typedef struct input input;
typedef struct asteroid asteroid;

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
////////////////
////////////////        Game logic
////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

static struct gamestate game;

void update_gamestate(gamestate* game){
    game->ship.x_pos += game->ship.x_speed;
    game->ship.y_pos += game->ship.y_speed;

    for(int i = 0; i < game->n_asteroids; i++){
        game->asteroids[i]->x_pos += game->asteroids[i]->x_speed;
        game->asteroids[i]->y_pos += game->asteroids[i]->y_speed;
    }
}

void update_ship(ship_object* ship, input input){
    if(input.turn_left){
        ship->orientation++;
    }
    else if(input.turn_right){
        ship->orientation--;
    }
    if(input.accelerate){
        // TODO: Update speeds
    }
}


void update_asteroid(asteroid* asteroid){

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
//struct asteroid* make_asteroid(int16_t x_speed, int16_t y_speed){
asteroid* make_asteroid(void){ 
    asteroid* new_asteroid = malloc(sizeof(asteroid));
    //asteroid->x_speed = x_speed;
    //asteroid->y_speed = y_speed;
    new_asteroid->x_speed = 10;
    new_asteroid->y_speed = 10;
    new_asteroid->x_pos = 100;
    new_asteroid->y_pos = 100;
    
    // Make asteroid poly

    return new_asteroid;
}

ship_object make_ship(void){
    ship_object new_ship;
    new_ship.x_speed = 0;
    new_ship.y_speed = 0;
    new_ship.x_pos = 100;
    new_ship.y_pos = 100;
    new_ship.orientation = 0;

    // Make ship poly

    return new_ship;
}

// Initializes the game struct
void init_logic(uint16_t n_asteroids, gamestate* game){
    game->ship = make_ship();
    game->asteroids = malloc(sizeof(asteroid*));
    for(int i = 0; i < n_asteroids; i++){
        game->asteroids[i] = make_asteroid();
    }
}