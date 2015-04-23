#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "game.h"
#include "util.h"
#include "logic.h"
#include "input.h"
#define DEBUG
#include "debug.h"
#include "spaceship.h"

#define PRINT_POSITION  false
#define PRINT_INPUT     false


///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
////////////////
////////////////        Game logic
////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

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
void init_asteroid(int n_coords, int32_t* x_coords, int32_t* y_coords, struct asteroid* asteroid, uint8_t size);
void do_logic();
void do_shoot(void);
void update_gamestate();
void update_projectiles();
void do_wrap(int32_t* x_pos, int32_t* y_pos);
void check_box_collisions(void);
bool check_bounding_box_collision(struct bounding_box* box1, struct bounding_box* box2);
bool check_poly_collision(polygon* p1, polygon* p2);
void print_ship_coords(int32_t x_pos, int32_t y_pos, int32_t x_speed, int32_t y_speed);
void print_bounding_box(struct bounding_box* box);
asteroid* spawn_asteroid(int32_t x_pos, int32_t y_pos, asteroid* asteroid);
void kill_asteroid(int index);
void print_asteroid_status();

// Variable prototypes. These are here to avoid cluttering important source
// code with long polygon definitions.
const int32_t asteroid1_n_coords;
const int32_t asteroid1_x_coords[];
const int32_t asteroid1_y_coords[]; 
const int32_t asteroid2_n_coords;
const int32_t asteroid2_x_coords[];
const int32_t asteroid2_y_coords[]; 
const int32_t asteroid3_n_coords;
const int32_t asteroid3_x_coords[];
const int32_t asteroid3_y_coords[]; 

// Global variables
struct gamestate game;
struct asteroid my_asteroids[MAX_AMOUNT_ASTEROIDS];
struct projectile my_projectiles[MAX_AMOUNT_PROJECTILES];


void do_logic(){
    uint8_t input = get_input();

    if(CHECK_PAUSE(input)){
        // Do pause
    }
    if(CHECK_DEBUG(input)){
      kill_asteroid(0);
    }

    update_spaceship(input);
    if(PRINT_POSITION){
      print_ship_coords(game.ship->x_pos,
          game.ship->y_pos,
          game.ship->x_speed,
          game.ship->y_speed);
    }

    if(CHECK_SHOOT(input)){
        do_shoot();
    }

    // print_bounding_box(&game.ship.collision_box);
    update_gamestate();
    update_projectiles();
    // Check collisions
}

void update_gamestate(){
    struct asteroid* current_asteroid;

    for(int i = 0; i < game.n_asteroids; i++){
        current_asteroid = game.active_asteroids[i];
        current_asteroid->x_pos += current_asteroid->x_speed;
        current_asteroid->y_pos += current_asteroid->y_speed;
        create_bounding_box(
            &current_asteroid->collision_box,
            &current_asteroid->poly
        );
    }
}

bool check_asteroid_spaceship_collision(struct asteroid* asteroid, struct spaceship* spaceship){
    struct bounding_box* box1 = &asteroid->collision_box;
    struct bounding_box* box2 = &spaceship->collision_box;
    return  INTERSECTS(
                box1->x_left_upper  + asteroid->x_pos,
                box1->x_right_lower + asteroid->x_pos,
                box2->x_left_upper  + spaceship->x_pos,
                box2->x_right_lower + spaceship->x_pos) && 
            INTERSECTS(
                box1->y_right_lower + asteroid->y_pos,
                box1->y_left_upper  + asteroid->y_pos,
                box2->y_right_lower + spaceship->y_pos,
                box2->y_left_upper  + spaceship->y_pos);
}
//bool check_bounding_box_collision(struct bounding_box* box1, struct bounding_box* box2){
//    return( INTERSECTS(box1->x_left_upper,  box1->x_right_lower, box2->x_left_upper,  box2->x_right_lower)
//        &&  INTERSECTS(box1->y_right_lower, box1->y_left_upper,  box2->y_right_lower, box2->y_left_upper));
//}

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
        if(check_asteroid_spaceship_collision(&game.asteroids[i], game.ship)){
            if(check_poly_collision(&game.ship->poly, &game.asteroids[i].poly)){
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
  if(game.n_asteroids <= 0){
    return;
  }
  game_debug("Attempting to kill asteroid %d\nCurrent status:\n", index);
  print_asteroid_status();
  if(game.active_asteroids[index]->type == SML){
    game_debug("Killing small asteroid\n");
    game.active_asteroids[index] = game.active_asteroids[--game.n_asteroids];
  }
  else if(game.active_asteroids[index]->type == MED){
    // Beware, lengthy expression!
    // Replaces the active asteroid pointer with a pointer to an unused asteroid. However, new asteroid needs to be initialized, and the x and y pos of the old 
    // asteroid is used. For the second asteroid the x and y pos values are basically daisy chained. Currently the two asteroids spawn on top of each others
    game_debug("Killing medium size asteroid\n");
    game.active_asteroids[index] = spawn_asteroid(
      game.active_asteroids[index]->x_pos, 
      game.active_asteroids[index]->y_pos, 
      &game.asteroids[LARGE_ASTEROIDS + MEDIUM_ASTEROIDS + game.n_sml_asteroids++]
    );
    game.active_asteroids[game.n_asteroids++] = spawn_asteroid(
      game.active_asteroids[index]->x_pos, 
      game.active_asteroids[index]->y_pos, 
      &game.asteroids[LARGE_ASTEROIDS + MEDIUM_ASTEROIDS + game.n_sml_asteroids++]
    );
  }
  else{ // LARGE
    game_debug("Killing large size asteroid\n");
    game.active_asteroids[index] = spawn_asteroid(
      game.active_asteroids[index]->x_pos, 
      game.active_asteroids[index]->y_pos, 
      &game.asteroids[LARGE_ASTEROIDS + game.n_med_asteroids++]
    );
    game.active_asteroids[game.n_asteroids++] = spawn_asteroid(
      game.active_asteroids[index]->x_pos, 
      game.active_asteroids[index]->y_pos, 
      &game.asteroids[LARGE_ASTEROIDS + game.n_med_asteroids++]
    );  
  }
  if(game.n_asteroids == 0){
    game_debug("YOU'RE WINNER!\n");
  }
  game_debug("Kill asteroid finished. n_asteroids: %d\n", game.n_asteroids);
  print_asteroid_status();
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
  if(size == BIG){
    new_asteroid->x_speed = RANDOM(-10,10)*SCREEN_TO_WORLD_RATIO;
    new_asteroid->y_speed = RANDOM(-10,10)*SCREEN_TO_WORLD_RATIO;
  }
  else if(size == MED){
    new_asteroid->x_speed = RANDOM(-20, 20)*SCREEN_TO_WORLD_RATIO;
    new_asteroid->y_speed = RANDOM(-20, 20)*SCREEN_TO_WORLD_RATIO;
  }
  else{
    new_asteroid->x_speed = RANDOM(-40,40)*SCREEN_TO_WORLD_RATIO;
    new_asteroid->y_speed = RANDOM(-40,40)*SCREEN_TO_WORLD_RATIO;
  }
}

// Initializes the module
int init_logic(struct gamestate** gamestate_ptr){
  game_debug("Initializing the logic module ...\n");

  // RANDOMNESS GUARANTEED!
  srand(2);

  // Initialize the gamestate struct
  init_spaceship(&game.ship);
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
        init_asteroid(
            asteroid1_n_coords,
            (int32_t*)&asteroid1_x_coords[0],
            (int32_t*)&asteroid1_y_coords[0],
            &game.asteroids[i],
            type
        );
      }
      else if(i < LARGE_ASTEROIDS + MEDIUM_ASTEROIDS){
        type = MED;
        init_asteroid(
            asteroid2_n_coords,
            (int32_t*)&asteroid2_x_coords[0],
            (int32_t*)&asteroid2_y_coords[0],
            &game.asteroids[i],
            type
        );
      }
      else{
        type = SML;
        init_asteroid(
            asteroid3_n_coords,
            (int32_t*)&asteroid3_x_coords[0],
            (int32_t*)&asteroid3_y_coords[0],
            &game.asteroids[i],
            type
        );
      }
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

void print_bounding_box(struct bounding_box* box){
  game_debug("Bounding box:\n");
  game_debug("left_upper: x: %d, y: %d, right_lower: x: %d, y: %d\n",
      box->x_left_upper/SCREEN_TO_WORLD_RATIO,
      box->y_left_upper/SCREEN_TO_WORLD_RATIO,
      box->x_right_lower/SCREEN_TO_WORLD_RATIO,
      box->y_right_lower/SCREEN_TO_WORLD_RATIO
  );
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
  game_debug("debug print active_asteroids. n_asteroids: %d\n", game.n_asteroids);
  for(int i = 0; i < game.n_asteroids; i++){
    if(game.active_asteroids[i] == NULL){
      game_debug("Asteroid %d is null\n", i);
    }
    else{
      game_debug("Asteroid %d has type %d\n", i, game.active_asteroids[i]->type);
    }
  }
  game_debug("Done scanning\n");
}

//-------------------------------------------
// Pre-defined polygons
//-------------------------------------------
const int32_t asteroid1_n_coords = 9;   // large asteroid
const int32_t asteroid1_x_coords[9] = {
  15 * SCREEN_TO_WORLD_RATIO, 
   0 * SCREEN_TO_WORLD_RATIO,
   0 * SCREEN_TO_WORLD_RATIO,
  10 * SCREEN_TO_WORLD_RATIO,  
  19 * SCREEN_TO_WORLD_RATIO, 
  29 * SCREEN_TO_WORLD_RATIO, 
  39 * SCREEN_TO_WORLD_RATIO, 
  39 * SCREEN_TO_WORLD_RATIO, 
  29 * SCREEN_TO_WORLD_RATIO, 
};
const int32_t asteroid1_y_coords[9] = { 
   0 * SCREEN_TO_WORLD_RATIO,  
  13 * SCREEN_TO_WORLD_RATIO,
  22 * SCREEN_TO_WORLD_RATIO,
  35 * SCREEN_TO_WORLD_RATIO, 
  35 * SCREEN_TO_WORLD_RATIO, 
  35 * SCREEN_TO_WORLD_RATIO, 
  23 * SCREEN_TO_WORLD_RATIO, 
  12 * SCREEN_TO_WORLD_RATIO, 
   0 * SCREEN_TO_WORLD_RATIO, 
};
const int32_t asteroid2_n_coords = 8;   // medium asteroid
const int32_t asteroid2_x_coords[8] = {
   6 * SCREEN_TO_WORLD_RATIO,
   0 * SCREEN_TO_WORLD_RATIO,
   0 * SCREEN_TO_WORLD_RATIO,  
   6 * SCREEN_TO_WORLD_RATIO, 
  13 * SCREEN_TO_WORLD_RATIO, 
  21 * SCREEN_TO_WORLD_RATIO, 
  21 * SCREEN_TO_WORLD_RATIO, 
  16 * SCREEN_TO_WORLD_RATIO, 
};
const int32_t asteroid2_y_coords[8] = { 
   0 * SCREEN_TO_WORLD_RATIO,
   5 * SCREEN_TO_WORLD_RATIO,
  12 * SCREEN_TO_WORLD_RATIO, 
  18 * SCREEN_TO_WORLD_RATIO, 
  18 * SCREEN_TO_WORLD_RATIO, 
  13 * SCREEN_TO_WORLD_RATIO, 
   4 * SCREEN_TO_WORLD_RATIO, 
   0 * SCREEN_TO_WORLD_RATIO, 
};
const int32_t asteroid3_n_coords = 9;   // small asteroid
const int32_t asteroid3_x_coords[9] = {
   4 * SCREEN_TO_WORLD_RATIO,
   0 * SCREEN_TO_WORLD_RATIO,
   0 * SCREEN_TO_WORLD_RATIO,
   3 * SCREEN_TO_WORLD_RATIO,  
   6 * SCREEN_TO_WORLD_RATIO, 
   8 * SCREEN_TO_WORLD_RATIO, 
  10 * SCREEN_TO_WORLD_RATIO, 
  10 * SCREEN_TO_WORLD_RATIO, 
   7 * SCREEN_TO_WORLD_RATIO, 
};
const int32_t asteroid3_y_coords[9] = { 
   0 * SCREEN_TO_WORLD_RATIO,
   4 * SCREEN_TO_WORLD_RATIO,
   7 * SCREEN_TO_WORLD_RATIO,
   9 * SCREEN_TO_WORLD_RATIO, 
   9 * SCREEN_TO_WORLD_RATIO, 
   9 * SCREEN_TO_WORLD_RATIO, 
   7 * SCREEN_TO_WORLD_RATIO, 
   3 * SCREEN_TO_WORLD_RATIO, 
   0 * SCREEN_TO_WORLD_RATIO, 
};
