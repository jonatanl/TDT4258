#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "game.h"
#include "util.h"
#include "logic.h"
#include "input.h"
// #define DEBUG
#include "debug.h"
#include "spaceship.h"
#include "asteroids.h"
#include "projectiles.h"

// Enable for debugging
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


// Static function prototypes
static void check_collisions(void);
static inline bool check_asteroid_spaceship_collision(struct asteroid* asteroid, struct spaceship* spaceship);
static inline bool check_asteroid_projectile_collision(struct asteroid* asteroid, struct projectile* projectile);

static void print_bounding_box(struct bounding_box* box);
static void print_ship_coords(int32_t x_pos, int32_t y_pos, int32_t x_speed, int32_t y_speed);


// Global variables
struct gamestate game;


void update_logic(){
  //game_debug("update_logic() called\n");
  uint8_t input = get_input();

  if(CHECK_DEBUG(input)){
    kill_asteroid(0);
  }

  update_asteroids();

  update_spaceship(input);
  if(PRINT_POSITION){
    print_ship_coords(game.ship->x_pos,
      game.ship->y_pos,
      game.ship->x_speed,
      game.ship->y_speed
    );
  }

  update_projectiles();
  if(CHECK_SHOOT(input)){
    do_shoot();
  }

  game.time++;

  game_debug("update_logic() done\n");
}

// Check for collisions
void check_collisions(void){

  for(int i=0; i<game.n_asteroids; i++){

    // Check asteroid-spaceship collisions
    if(check_asteroid_spaceship_collision(game.active_asteroids[i], game.ship)){
      // TODO: Game over
      // TODO: Check polygon collisions
      game_debug("GAME OVER MAN, GAME OVER\n");
    }

    // Check asteroid-projectile collisions
    for(int j=0; j<game.n_projectiles; j++){
      if(check_asteroid_projectile_collision(game.active_asteroids[i], game.active_projectiles[j])){
        kill_asteroid(i);
        kill_projectile(j);
      }
    }
  }
}



static inline bool check_asteroid_spaceship_collision(struct asteroid* asteroid, struct spaceship* spaceship){
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

static inline bool check_asteroid_projectile_collision(struct asteroid* asteroid, struct projectile* projectile){
  struct bounding_box* box1 = &asteroid->collision_box;
  return  POINT_LINE_INTERSECTS(
              projectile->x_pos,
              box1->x_left_upper  + asteroid->x_pos,
              box1->x_right_lower + asteroid->x_pos) && 
          POINT_LINE_INTERSECTS(
              projectile->y_pos,
              box1->y_right_lower + asteroid->y_pos,
              box1->y_left_upper  + asteroid->y_pos
              );
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

// Initializes the module
int init_logic(struct gamestate** gamestate_ptr){
  game_debug("Initializing the logic module ...\n");

  // Seed random generator
  srand(2);

  // Reset game time
  game.time = 0;

  // Initialize submodules
  init_spaceship(&game.ship);
  game_debug("Initializing asteroids\n");
  init_asteroids(&game);
  game_debug("Done initializing asteroids\n");
  if(game.active_asteroids == NULL){

  }
  init_projectiles(&game);
  //init_asteroids(&game);
  //init_projectiles(&game);

  // Return a pointer to the gamestate structure
  *gamestate_ptr = &game;

  // No errors
  game_debug("DONE: No errors initializing the logic module\n");
  return 0;
}

// Releases the module
int release_logic(){
  game_debug("Releasing the logic module ...\n");

  // Release submodules
  release_spaceship();
  //release_asteroids();
  //release_projectiles();

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
