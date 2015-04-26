#include "asteroids.h"
#include "input.h"
#include "logic.h"
#include "util.h"
// #define DEBUG
#include "debug.h"
#include "draw.h"

#include <stdbool.h>
#include <stdlib.h>

#define BIG 2
#define MED 1
#define SML 0


// Prototypes
static struct gamestate* game;
static struct asteroid my_asteroids[MAX_AMOUNT_ASTEROIDS];
static struct asteroid* spawn_asteroid(int32_t x_pos, int32_t y_pos, asteroid* asteroid);
static void print_asteroid_status(void);
void kill_asteroid_id(int id);

static const int32_t asteroid1_n_coords;
static const int32_t asteroid1_x_coords[];
static const int32_t asteroid1_y_coords[]; 
static const int32_t asteroid2_n_coords;
static const int32_t asteroid2_x_coords[];
static const int32_t asteroid2_y_coords[]; 
static const int32_t asteroid3_n_coords;
static const int32_t asteroid3_x_coords[];
static const int32_t asteroid3_y_coords[]; 

// Adds world coordinates to an asteroid
static struct asteroid* spawn_asteroid(int32_t x_pos, int32_t y_pos, asteroid* asteroid){
  asteroid->x_pos = x_pos;
  asteroid->y_pos = y_pos;
  return asteroid;
}

void update_asteroids(){
  struct asteroid* current_asteroid;

  for(int i = 0; i < game->n_asteroids; i++){
    current_asteroid = game->active_asteroids[i];
    current_asteroid->x_pos += current_asteroid->x_speed;
    current_asteroid->y_pos += current_asteroid->y_speed;
    wrap_coordinate(&current_asteroid->x_pos, &current_asteroid->y_pos);
    create_bounding_box(
      &current_asteroid->collision_box,
      &current_asteroid->poly
    );
  }
}

// Handles a killed asteroid. Spawns 2 smaller asteroids if big asteroid is killed
// Also handles list of active asteroids
void kill_asteroid(int index){
  if(game->n_asteroids <= 0){
    return;
  }
  game_debug("Attempting to kill asteroid %d\nCurrent status:\n", index);
  print_asteroid_status();

  // Update framebuffer to remove old asteroid
  do_update_moving_asteroid(game->active_asteroids[index]);

  if(game->active_asteroids[index]->type == SML){
    game_debug("Killing small asteroid\n");
    game->active_asteroids[index] = game->active_asteroids[--game->n_asteroids];
  }
  else if(game->active_asteroids[index]->type == MED){
    // Beware, lengthy expression!
    // Replaces the active asteroid pointer with a pointer to an unused asteroid. However, new asteroid needs to be initialized, and the x and y pos of the old 
    // asteroid is used. For the second asteroid the x and y pos values are basically daisy chained. Currently the two asteroids spawn on top of each others
    game_debug("Killing medium size asteroid\n");
    game->active_asteroids[index] = spawn_asteroid(
      game->active_asteroids[index]->x_pos, 
      game->active_asteroids[index]->y_pos, 
      &my_asteroids[LARGE_ASTEROIDS + MEDIUM_ASTEROIDS + game->n_sml_asteroids++]
    );
    game->active_asteroids[game->n_asteroids++] = spawn_asteroid(
      game->active_asteroids[index]->x_pos, 
      game->active_asteroids[index]->y_pos, 
      &my_asteroids[LARGE_ASTEROIDS + MEDIUM_ASTEROIDS + game->n_sml_asteroids++]
    );
  }
  else{ // LARGE
    game_debug("Killing large size asteroid\n");
    game->active_asteroids[index] = spawn_asteroid(
      game->active_asteroids[index]->x_pos, 
      game->active_asteroids[index]->y_pos, 
      &my_asteroids[LARGE_ASTEROIDS + game->n_med_asteroids++]
    );
    game->active_asteroids[game->n_asteroids++] = spawn_asteroid(
      game->active_asteroids[index]->x_pos, 
      game->active_asteroids[index]->y_pos, 
      &my_asteroids[LARGE_ASTEROIDS + game->n_med_asteroids++]
    );  
  }
  if(game->n_asteroids == 0){
    game_debug("YOU'RE WINNER!\n");
  }
  game_debug("Kill asteroid finished. n_asteroids: %d\n", game->n_asteroids);
  print_asteroid_status();
}

// Figures out which index corresponds to the id
void kill_asteroid_id(int id){
  for(int i = 0; i < game->n_asteroids; i++){
    if(id == game->active_asteroids[i]->id){
      kill_asteroid(i);
      return;
    }
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

static void init_asteroid(int n_coords, 
                          int32_t* x_coords, 
                          int32_t* y_coords, 
                          struct asteroid* new_asteroid, 
                          uint8_t size, 
                          int32_t id_number
  ){ 
  new_asteroid->id = id_number;
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

void init_asteroids(gamestate* game_ptr){
  game_debug("Initializing asteroids ... \n");

  game = game_ptr;
  game->n_asteroids = START_ASTEROIDS;
  game->n_big_asteroids = START_ASTEROIDS;
  game->n_med_asteroids = 0;
  game->n_sml_asteroids = 0;
  uint8_t type;
  for(int i = 0; i < MAX_AMOUNT_ASTEROIDS; i++){      
    if(i < LARGE_ASTEROIDS){
      type = BIG;
      init_asteroid(
        asteroid1_n_coords,
        (int32_t*)&asteroid1_x_coords[0],
        (int32_t*)&asteroid1_y_coords[0],
        &my_asteroids[i],
        type,
        i
      );
    }
    else if(i < LARGE_ASTEROIDS + MEDIUM_ASTEROIDS){
      type = MED;
      init_asteroid(
        asteroid2_n_coords,
        (int32_t*)&asteroid2_x_coords[0],
        (int32_t*)&asteroid2_y_coords[0],
        &my_asteroids[i],
        type,
        i
      );
    }
    else{
      type = SML;
      init_asteroid(
        asteroid3_n_coords,
        (int32_t*)&asteroid3_x_coords[0],
        (int32_t*)&asteroid3_y_coords[0],
        &my_asteroids[i],
        type,
        i
      );
    }
  }

  // Initialized the pointer list to alive asteroids, and populates it with the initial asteroids
  game_debug("initializing active asteroids pointer\n");
  game->active_asteroids = malloc(sizeof(asteroid*)*MAX_ALIVE_ASTEROIDS);
  for(int i = 0; i < START_ASTEROIDS; i++){
    if(&my_asteroids[i] == NULL){game_debug("made pointer to NULL asteroid");}
    game->active_asteroids[i] = &my_asteroids[i];
    spawn_asteroid((10 + i*20 *SCREEN_TO_WORLD_RATIO), (40 + i*20 *SCREEN_TO_WORLD_RATIO), game->active_asteroids[i]);
  }

  if(game->active_asteroids == NULL){
    game_debug("NULL asteroids\n");
  }
  else{
    game_debug("game->active_asteroids is not a NULL ptr\n");  
  }

  for(int i = 0; i < START_ASTEROIDS; i++){
    if(game->active_asteroids[i] == NULL){
      game_debug("NULL asteroid[%d]\n", i);
    }
  }

  game_debug("Done initializing asteroids\n");
}

static void release_asteroids(){
  free(game->active_asteroids);
}

//-------------------------------------------
// Pre-defined polygons
//-------------------------------------------
static const int32_t asteroid1_n_coords = 9;   // large asteroid
static const int32_t asteroid1_x_coords[9] = {
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
static const int32_t asteroid1_y_coords[9] = { 
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
static const int32_t asteroid2_n_coords = 8;   // medium asteroid
static const int32_t asteroid2_x_coords[8] = {
   6 * SCREEN_TO_WORLD_RATIO,
   0 * SCREEN_TO_WORLD_RATIO,
   0 * SCREEN_TO_WORLD_RATIO,  
   6 * SCREEN_TO_WORLD_RATIO, 
  13 * SCREEN_TO_WORLD_RATIO, 
  21 * SCREEN_TO_WORLD_RATIO, 
  21 * SCREEN_TO_WORLD_RATIO, 
  16 * SCREEN_TO_WORLD_RATIO, 
};
static const int32_t asteroid2_y_coords[8] = { 
   0 * SCREEN_TO_WORLD_RATIO,
   5 * SCREEN_TO_WORLD_RATIO,
  12 * SCREEN_TO_WORLD_RATIO, 
  18 * SCREEN_TO_WORLD_RATIO, 
  18 * SCREEN_TO_WORLD_RATIO, 
  13 * SCREEN_TO_WORLD_RATIO, 
   4 * SCREEN_TO_WORLD_RATIO, 
   0 * SCREEN_TO_WORLD_RATIO, 
};
static const int32_t asteroid3_n_coords = 9;   // small asteroid
static const int32_t asteroid3_x_coords[9] = {
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
static const int32_t asteroid3_y_coords[9] = { 
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

////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////
////////////    DEBUG
////////////
////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////

static void print_asteroid_status(){
  game_debug("debug print active_asteroids. n_asteroids: %d\n", game->n_asteroids);
  for(int i = 0; i < game->n_asteroids; i++){
    if(game->active_asteroids[i] == NULL){
      game_debug("Asteroid %d is null\n", i);
    }
    else{
      game_debug("Asteroid %d has type %d\n", i, game->active_asteroids[i]->type);
    }
  }
  game_debug("Done scanning\n");
}
