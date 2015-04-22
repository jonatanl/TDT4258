#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "game.h"
#include "util.h"
#include "logic.h"
#include "input.h"
#define DEBUG
#include "debug.h"
#include "spaceship.h"

#define PRINT_POSITION  false
#define PRINT_INPUT     true


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

#define LARGE_BRANCH            2
#define MEDIUM_BRANCH           2

#define START_ASTEROIDS         3
#define LARGE_ASTEROIDS         START_ASTEROIDS
#define MEDIUM_ASTEROIDS        START_ASTEROIDS*LARGE_BRANCH
#define SMALL_ASTEROIDS         MEDIUM_ASTEROIDS*MEDIUM_BRANCH

#define MAX_AMOUNT_ASTEROIDS    LARGE_ASTEROIDS + MEDIUM_ASTEROIDS + SMALL_ASTEROIDS  
#define MAX_ALIVE_ASTEROIDS     LARGE_ASTEROIDS*LARGE_BRANCH*MEDIUM_BRANCH
#define MAX_AMOUNT_PROJECTILES  10

#define BIG 2
#define MED 1
#define SML 0

// Function prototypes
void init_ship(struct ship_object* ship);
void init_asteroid(int n_coords, int32_t* x_coords, int32_t* y_coords, struct asteroid* asteroid, uint8_t size);
void do_logic();
void do_shoot(void);
void update_ship();
void update_gamestate();
void update_projectiles();
void do_wrap(int32_t* x_pos, int32_t* y_pos);
void update_bounding_box(polygon* poly, int32_t x_abs, int32_t y_abs);
void check_box_collisions(void);
bool check_bounding_box_collision(polygon* p1, polygon* p2);
bool check_poly_collision(polygon* p1, polygon* p2);
void print_ship_coords(int32_t x_pos, int32_t y_pos, int32_t x_speed, int32_t y_speed);
void print_poly_bb(polygon* poly);
asteroid* spawn_asteroid(int32_t x_pos, int32_t y_pos, asteroid* asteroid);
void kill_asteroid(int index);

// Variable prototypes
const int32_t asteroid1_n_coords;
const int32_t asteroid1_x_coords[];
const int32_t asteroid1_y_coords[]; 

// Global variables
struct gamestate game;
struct asteroid my_asteroids[MAX_AMOUNT_ASTEROIDS];
struct projectile my_projectiles[MAX_AMOUNT_PROJECTILES];


void do_logic(){

    update_ship();
    update_gamestate();
    update_projectiles();
    // Check collisions
}

void update_gamestate(){
    game.ship.x_pos = game.ship.x_pos + game.ship.x_speed;
    game.ship.y_pos = game.ship.y_pos + game.ship.y_speed;
    do_wrap(&game.ship.x_pos, &game.ship.y_pos);

    if(PRINT_POSITION){
        game_debug("ship xpos: %d, ship ypos: %d\n", game.ship.x_pos, game.ship.y_speed);
    }

    for(int i = 0; i < game.n_asteroids; i++){
        game.active_asteroids[i]->x_pos += game.active_asteroids[i]->x_speed;
        game.active_asteroids[i]->y_pos += game.active_asteroids[i]->y_speed;
        update_bounding_box(&game.active_asteroids[i]->poly, game.active_asteroids[i]->x_pos, game.active_asteroids[i]->y_pos);
    }
}


void update_bounding_box(polygon* poly, int32_t x_abs, int32_t y_abs){
  
    poly->x_left_upper = x_abs + poly->x_coords[0];
    poly->x_right_lower = x_abs + poly->x_coords[0];
    poly->y_left_upper = y_abs + poly->y_coords[0];
    poly->y_right_lower = y_abs + poly->y_coords[0];
    
    for(int i = 1; i < poly->n_vertices; i++){
        poly->x_left_upper = ARG_MAX(poly->x_coords[i] + x_abs, poly->x_left_upper);
        poly->x_right_lower = ARG_MIN(poly->x_coords[i] + x_abs, poly->x_right_lower);
        poly->y_left_upper = ARG_MIN(poly->y_coords[i] + y_abs, poly->y_left_upper);
        poly->y_right_lower = ARG_MAX(poly->y_coords[i] + y_abs, poly->y_right_lower);
    }
}

bool check_bounding_box_collision(polygon* p1, polygon* p2){    
    return( INTERSECTS(p1->x_left_upper, p1->x_right_lower, p2->x_left_upper, p2->x_right_lower)
        &&  INTERSECTS(p1->y_right_lower, p1->y_left_upper, p2->y_right_lower, p2->y_left_upper));
}

// Handles input
void update_ship(){

    uint8_t input = get_input();
    update_bounding_box(&game.ship.poly, game.ship.x_pos, game.ship.y_pos);

    print_poly_bb(&game.ship.poly);

    if(CHECK_PAUSE(input)){
        // Do pause
    }
    

    if(PRINT_INPUT){
        static uint8_t prev_input = 0;
        if(input != prev_input){
            prev_input = input;
            game_debug("Input registered, %d\n", input);

            if(!(CHECK_LEFT(input) && CHECK_RIGHT(input))){ 
                game_debug("Registered no left/right conflict\n");

                if(CHECK_LEFT(input)){
                    game_debug("Registered left turn\n");

                }
                else if(CHECK_RIGHT(input)){
                    game_debug("registered right turn\n");
                }
            }
            if(CHECK_ACC(input)){
                game_debug("Registered acceleration\n");
            }
        }        
    }
    // END DEBUG STUFF

    // If both left and right is pressed the ship does nothing
    // if else, check if turn, do roation, and normalize
    if(!(CHECK_LEFT(input) && CHECK_RIGHT(input))){   
        
        if(CHECK_LEFT(input)){
            // TODO rotation
            rotate_counterclockwise(&game.ship);
        }
        else if(CHECK_RIGHT(input)){
            // TODO rotation
            rotate_clockwise(&game.ship);
        }
    }
    if(CHECK_ACC(input)){
        // TODO orientation
        game.ship.x_speed += game.ship.x_orientation * DEFAULT_ACCELERATION;
        game.ship.y_speed += game.ship.y_orientation * DEFAULT_ACCELERATION;
    }

    if(CHECK_DEBUG(input)){
      // When this button is pressed you're gonna see some serious seg faults
      game_debug("## ## DEBUG ## ## \nATTEMPTING TO KILL ASTEROID\n ## ## DEBUG ## ##\n");
      kill_asteroid(0);
    }

    // TODO: Update speeds
    // Decrements the gun cooldown, or checks if shoot is pressed and fires a shot
    if(game.ship.gun_cooldown){
        game.ship.gun_cooldown--;
    }
    else if(CHECK_SHOOT(input)){
        do_shoot();
    }

    game.ship.x_pos += game.ship.x_speed;
    game.ship.y_pos += game.ship.y_speed;
    
    print_ship_coords(game.ship.x_pos,
        game.ship.y_pos,
        game.ship.x_speed,
        game.ship.y_speed);
}

void update_projectiles() {
  for (int i = 0; i < game.n_projectiles; ++i) {
    game.projectiles[i].x_pos += game.projectiles[i].x_speed;
    game.projectiles[i].y_pos += game.projectiles[i].y_speed;
  }
}

void do_wrap(int32_t* x_pos, int32_t* y_pos){
    if(*x_pos >= DEFAULT_WORLD_X_DIM){
        *x_pos -= DEFAULT_WORLD_X_DIM; 
    }
    else if(*x_pos < 0){
        *x_pos += DEFAULT_WORLD_X_DIM;
    }
    if(*y_pos >= DEFAULT_WORLD_Y_DIM){
        *y_pos -= DEFAULT_WORLD_Y_DIM; 
    }
    else if(*y_pos < 0){
        *y_pos += DEFAULT_WORLD_Y_DIM;
    }
}

void check_box_collisions(){
    for(int i = 0; i < game.n_asteroids; i++){
        if(check_bounding_box_collision(&game.ship.poly, &game.asteroids[i].poly)){
            if(check_poly_collision(&game.ship.poly, &game.asteroids[i].poly)){
                game_debug("GAME OVER MAN, GAME OVER\n");
            }
        }
    }
}



bool check_poly_collision(polygon* p1, polygon* p2){
    return true;
}

void do_shoot(void){

}

// Handles a killed asteroid. Spawns 2 smaller asteroids if big asteroid is killed
// Also handles list of active asteroids
void kill_asteroid(int index){
  if(game.active_asteroids[index]->type == SML){
    game.active_asteroids[index] = game.active_asteroids[game.n_asteroids--];
  }
  else if(game.active_asteroids[index]->type == MED){
    // Beware, lengthy expression!
    // Replaces the active asteroid pointer with a pointer to an unused asteroid. However, new asteroid needs to be initialized, and the x and y pos of the old 
    // asteroid is used. For the second asteroid the x and y pos values are basically daisy chained. Currently the two asteroids spawn on top of each others
    game.active_asteroids[index] = spawn_asteroid(game.active_asteroids[index]->x_pos, game.active_asteroids[index]->y_pos, &game.asteroids[START_ASTEROIDS*3 + game.n_sml_asteroids++]);
    game.active_asteroids[++game.n_asteroids] = spawn_asteroid(game.active_asteroids[index]->x_pos, game.active_asteroids[index]->y_pos, &game.asteroids[START_ASTEROIDS*3 + game.n_sml_asteroids++]);
  }
  else{
    game.active_asteroids[index] = spawn_asteroid(game.active_asteroids[index]->x_pos, game.active_asteroids[index]->y_pos, &game.asteroids[START_ASTEROIDS + game.n_med_asteroids++]);
    game.active_asteroids[++game.n_asteroids] = spawn_asteroid(game.active_asteroids[index]->x_pos, game.active_asteroids[index]->y_pos, &game.asteroids[START_ASTEROIDS + game.n_med_asteroids++]);  
  }
  if(game.n_asteroids == 0){
    game_debug("YOU'RE WINNER!\n");
  }
}

// Adds world coordinates to an asteroid
asteroid* spawn_asteroid(int32_t x_pos, int32_t y_pos, asteroid* asteroid){
  asteroid->x_pos = x_pos;
  asteroid->y_pos = y_pos;
  return asteroid;
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
void init_asteroid(int n_coords, int32_t* x_coords, int32_t* y_coords, struct asteroid* new_asteroid, uint8_t size){ 
  
  // Initialize the asteroid polygon
  new_asteroid->poly.n_vertices = n_coords;
  new_asteroid->poly.x_coords = x_coords;
  new_asteroid->poly.y_coords = y_coords;
  new_asteroid->type = size;
}

// Initialize a spaceship
void init_ship(struct ship_object* ship){
    ship->x_speed = 0;
    ship->y_speed = 0;
    ship->x_pos = DEFAULT_WORLD_X_DIM / 2;
    ship->y_pos = DEFAULT_WORLD_Y_DIM / 2;
    ship->x_orientation = 0;
    ship->y_orientation = 1;
    ship->gun_cooldown = 0;

    // these values are probably pretty bad
    // TODO rethink this
    ship->poly.n_vertices = 3;
    int32_t* x_coords = malloc(sizeof(int32_t)*3);
    int32_t* y_coords = malloc(sizeof(int32_t)*3);
    
    // Add vertices for a spaceship polygon
    x_coords[0] =  0 * SCREEN_TO_WORLD_RATIO;
    y_coords[0] =  9 * SCREEN_TO_WORLD_RATIO;
    x_coords[1] =  7 * SCREEN_TO_WORLD_RATIO;
    y_coords[1] = -6 * SCREEN_TO_WORLD_RATIO;
    x_coords[2] = -7 * SCREEN_TO_WORLD_RATIO;
    y_coords[2] = -6 * SCREEN_TO_WORLD_RATIO;

    ship->poly.x_coords = x_coords;
    ship->poly.y_coords = y_coords;
}

// Initializes the module
int init_logic(struct gamestate** gamestate_ptr){
  game_debug("Initializing the logic module ...\n");

  // Initialize the gamestate struct
  init_ship(&game.ship);
  game.asteroids = &my_asteroids[0];
  game.n_asteroids = START_ASTEROIDS;
  game.n_big_asteroids = START_ASTEROIDS;
  game.n_med_asteroids = 0;
  game.n_sml_asteroids = 0;
  game.projectiles = &my_projectiles[0];
  game.n_projectiles = 0;
  game.world_x_dim = DEFAULT_WORLD_X_DIM;
  game.world_y_dim = DEFAULT_WORLD_Y_DIM;

  // Initialize all asteroids
  uint8_t type;
  for(int i = 0; i < MAX_AMOUNT_ASTEROIDS; i++){      
      if(i < LARGE_ASTEROIDS){
        type = BIG;
      }
      else if(i < LARGE_ASTEROIDS + MEDIUM_ASTEROIDS){
        type = MED;
      }
      else{
        type = SML;
      }

      // Initialize asteroids
      //
      // TODO: Use different polygons for different asteroids
      init_asteroid(3, (int32_t*)&asteroid1_x_coords[0], (int32_t*)&asteroid1_y_coords[0], &game.asteroids[i], type);
  }

  // Initialized the pointer list to alive asteroids, and populates it with the initial asteroids
  game.active_asteroids = malloc(sizeof(asteroid*)*MAX_ALIVE_ASTEROIDS);
  for(int i = 0; i < START_ASTEROIDS; i++){
    game.active_asteroids[i] = &game.asteroids[i];
    spawn_asteroid((10 + i*20 *SCREEN_TO_WORLD_RATIO), (40 + i*20 *SCREEN_TO_WORLD_RATIO), game.active_asteroids[i]);
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

////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////
////////////    DEBUG
////////////
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////

void print_poly_bb(polygon* poly){
  game_debug("Bounding box:\n");
  game_debug("left_upper: x: %d, y: %d, right_lower: x: %d, y: %d\n", poly->x_left_upper/SCREEN_TO_WORLD_RATIO, poly->y_left_upper/SCREEN_TO_WORLD_RATIO, poly->x_right_lower/SCREEN_TO_WORLD_RATIO, poly->y_right_lower/SCREEN_TO_WORLD_RATIO);
}

void print_ship_coords(int32_t x_pos, int32_t y_pos, int32_t x_speed, int32_t y_speed){
  game_debug("spaceship: px = %d, py = %d, sx = %d, sy = %d\n",
    x_pos,
    y_pos,
    x_speed,
    y_speed
  );
}

void print_asteroid_status(){

}


//-------------------------------------------
// Pre-defined polygon coordinates in pixels
//-------------------------------------------
const int32_t asteroid1_n_coords = 9;
const int32_t asteroid1_x_coords[9] = {
  15 * SCREEN_TO_WORLD_RATIO, 
  29 * SCREEN_TO_WORLD_RATIO, 
  39 * SCREEN_TO_WORLD_RATIO, 
  39 * SCREEN_TO_WORLD_RATIO, 
  29 * SCREEN_TO_WORLD_RATIO, 
  19 * SCREEN_TO_WORLD_RATIO, 
  10 * SCREEN_TO_WORLD_RATIO,  
   0 * SCREEN_TO_WORLD_RATIO,
   0 * SCREEN_TO_WORLD_RATIO
};
const int32_t asteroid1_y_coords[9] = { 
   0 * SCREEN_TO_WORLD_RATIO,  
   0 * SCREEN_TO_WORLD_RATIO, 
  12 * SCREEN_TO_WORLD_RATIO, 
  23 * SCREEN_TO_WORLD_RATIO, 
  35 * SCREEN_TO_WORLD_RATIO, 
  35 * SCREEN_TO_WORLD_RATIO, 
  35 * SCREEN_TO_WORLD_RATIO, 
  22 * SCREEN_TO_WORLD_RATIO,
  13 * SCREEN_TO_WORLD_RATIO
};
