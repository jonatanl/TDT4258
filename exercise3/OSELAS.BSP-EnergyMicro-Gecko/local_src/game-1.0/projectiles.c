#include "projectiles.h"
#include "input.h"
#include "logic.h"
#include "util.h"
#include "debug.h"

#include <stdbool.h>
#include <stdlib.h>

// Prototypes
static void spawn_projectile();

// Global variables
static struct gamestate* game;
static struct projectile my_projectiles[MAX_AMOUNT_PROJECTILES];
static int free_spots[MAX_AMOUNT_PROJECTILES] = {0};

void do_shoot(void){
  if(game->n_projectiles >= MAX_AMOUNT_PROJECTILES){
    return;
  }
  spawn_projectile();
}

// Spawns and inserts a projectile
// Since this function is somewhat obtuse it is commented liberally
static void spawn_projectile(){
  int index = -1;
  for(index = 0; index < MAX_AMOUNT_PROJECTILES; index++){
    if(free_spots[index] == 0){
      free_spots[index] = 1;
      break;
    }
  }
  if(index == -1){game_debug("No free projectile spot found, this should not happen\n");}

  projectile* projectile = &my_projectiles[index];
  projectile->x_pos = game->ship->x_pos;
  projectile->y_pos = game->ship->y_pos;

  projectile->x_speed = (int)game->ship->x_orientation*SCREEN_TO_WORLD_RATIO*30;
  projectile->y_speed = (int)game->ship->y_orientation*SCREEN_TO_WORLD_RATIO*30;

  //projectile->lifetime = PROJECTILE_LIFETIME;

  game->active_projectiles[game->n_projectiles++] = projectile;
}

static void kill_projectile(int index){
  if(game->n_projectiles <= 0){
    return;
  }
  free_spots[game->active_projectiles[index] - my_projectiles] = 0;
  game->active_projectiles[index] = game->active_projectiles[--game->n_projectiles];
}

void update_projectiles(){
  game_debug("Attempting to update projectile despawns\n");
  for (int i = game->n_projectiles; i > 0; --i) {
    //if(game->active_projectiles[i]->lifetime-- == 0){
    //  kill_projectile(i);
    //}
  }
  game_debug("Attempting to update projectile positions\n");
  for(int i = 0; i < game->n_projectiles; i++){
    game->active_projectiles[i]->x_pos += game->active_projectiles[i]->x_speed;
    game->active_projectiles[i]->y_pos += game->active_projectiles[i]->y_speed;
  }
  game_debug("Done updating projectiles\n");
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

void init_projectiles(gamestate* game_ptr){
  game = game_ptr;
  game->active_projectiles = malloc(sizeof(projectile*)*MAX_AMOUNT_PROJECTILES);
  game->n_projectiles = 0;
}
