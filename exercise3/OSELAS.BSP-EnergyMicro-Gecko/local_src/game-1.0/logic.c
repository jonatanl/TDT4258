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
#include "asteroids.h"
#include "projectiles.h"

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


// Function prototypes
void init_asteroid(int n_coords, int32_t* x_coords, int32_t* y_coords, struct asteroid* asteroid, uint8_t size);
void do_logic();
void do_shoot(void);
void update_gamestate(uint8_t input);
void update_projectiles();
void check_box_collisions(void);
bool check_bounding_box_collision(struct bounding_box* box1, struct bounding_box* box2);
bool check_poly_collision(polygon* p1, polygon* p2);
void print_ship_coords(int32_t x_pos, int32_t y_pos, int32_t x_speed, int32_t y_speed);
void print_bounding_box(struct bounding_box* box);
asteroid* spawn_asteroid(int32_t x_pos, int32_t y_pos, asteroid* asteroid);
void kill_asteroid(int index);
void print_asteroid_status();
void spawn_projectile(void);
void kill_projectile(int index);

// Global variables
struct gamestate game;

void do_logic(){
  uint8_t input = get_input();

  if(CHECK_PAUSE(input)){
      // Do pause
  }
  if(CHECK_DEBUG(input)){
    kill_asteroid(0);
  }

  if(PRINT_POSITION){
    print_ship_coords(game.ship->x_pos,
      game.ship->y_pos,
      game.ship->x_speed,
      game.ship->y_speed
    );
  }

  if(CHECK_SHOOT(input)){
    do_shoot();
  }

  update_gamestate(input);
}

void update_gamestate(uint8_t input){
  update_projectiles();
  update_asteroids();
  update_spaceship(input);
  // Check collisions

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

  // RANDOMNESS GUARANTEED!
  srand(2);

  // Initialize the gamestate struct, and all the submodules for tracking gamestate
  init_spaceship(&game.ship);
  init_asteroids(&game);
  init_projectiles(&game);
  init_asteroids(&game);
  game.world_x_dim = DEFAULT_WORLD_X_DIM;
  game.world_y_dim = DEFAULT_WORLD_Y_DIM;

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

