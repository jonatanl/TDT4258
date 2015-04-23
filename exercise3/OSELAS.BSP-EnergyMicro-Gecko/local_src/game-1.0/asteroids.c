#include "asteroids.h"
#include "input.h"
#include "logic.h"
#include "util.h"
#include "debug.h"

#include <stdbool.h>
#include <stdlib.h>

#define BIG 2
#define MED 1
#define SML 0

struct asteroid my_asteroids[MAX_AMOUNT_ASTEROIDS];
extern struct gamestate game;

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

// Adds world coordinates to an asteroid
asteroid* spawn_asteroid(int32_t x_pos, int32_t y_pos, asteroid* asteroid){
  asteroid->x_pos = x_pos;
  asteroid->y_pos = y_pos;
  return asteroid;
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

void init_asteroids(){
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
