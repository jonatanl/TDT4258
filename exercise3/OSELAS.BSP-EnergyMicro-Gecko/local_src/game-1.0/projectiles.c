#include "projectiles.h"
#include "input.h"
#include "logic.h"
#include "util.h"
// #define DEBUG
#include "debug.h"

#include <stdbool.h>
#include <stdlib.h>

// Prototypes
static void spawn_projectile();
void print_projectiles_status(void);

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

  projectile* projectile = &my_projectiles[index];
  projectile->x_pos = game->ship->x_pos;
  projectile->y_pos = game->ship->y_pos;

  projectile->x_speed = (int)(game->ship->x_orientation*-SCREEN_TO_WORLD_RATIO*10);
  projectile->y_speed = (int)(game->ship->y_orientation*-SCREEN_TO_WORLD_RATIO*10);

  projectile->lifetime = PROJECTILE_LIFETIME;

  projectile->poly.x_coords[0] = 0;
  projectile->poly.x_coords[1] = (int)(game->ship->x_orientation*SCREEN_TO_WORLD_RATIO*2);

  projectile->poly.y_coords[0] = 0;
  projectile->poly.y_coords[1] = (int)(game->ship->y_orientation*SCREEN_TO_WORLD_RATIO*2);

  game->active_projectiles[game->n_projectiles++] = projectile;
}


void kill_projectile(int index){
  if(game->n_projectiles < index){
    game_debug("kill projectile issued on non existing projectile %d\n", index);
  }
  if(game->n_projectiles <= 0){
    return;
  }
  free_spots[game->active_projectiles[index] - my_projectiles] = 0;
  game->active_projectiles[index] = game->active_projectiles[--game->n_projectiles];
}

void kill_projectile_id(int id){
  for(int i = 0; i < game->n_projectiles; i++){
    if(id == game->active_projectiles[i]->id){
      kill_projectile(i);
      return;
    }
  }
}

void update_projectiles(){
  game_debug("Attempting to update projectile despawns\n");
  // print_projectiles_status();
  for (int i = game->n_projectiles; i >= 0; --i) {
    if(game->active_projectiles[i]->lifetime-- == 0){
      kill_projectile(i);
      game_debug("Despawned projectile\n");
    }
  }
  game_debug("Attempting to update projectile positions\n");
  for(int i = 0; i < game->n_projectiles; i++){
    game->active_projectiles[i]->x_pos += game->active_projectiles[i]->x_speed;
    game->active_projectiles[i]->y_pos += game->active_projectiles[i]->y_speed;
    wrap_coordinate(&game->active_projectiles[i]->x_pos, &game->active_projectiles[i]->y_pos);
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
  for(int i = 0; i < MAX_AMOUNT_PROJECTILES; i++){
    my_projectiles[i].poly.x_coords = malloc(sizeof(int32_t)*4);
    my_projectiles[i].poly.y_coords = malloc(sizeof(int32_t)*4);
    my_projectiles[i].poly.n_vertices = 2;
    my_projectiles[i].id = i;
  }
}

void release_projectiles(){
  free(game->active_projectiles);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
////////////////
////////////////        DEBUG
////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////


void print_projectiles_status(){
  game_debug("## PROJECTILE STATUS: ##\n");
  game_debug("game->n_projectiles: %d\n", game->n_projectiles);
  game_debug("Free spots:\n");
  for(int i = 0; i < MAX_AMOUNT_PROJECTILES; i++){
    if(free_spots[i] == 0){
      game_debug("[o]");
    }
    else{
      game_debug("[x]");
    }
  }
  game_debug("\n\nTesting for null ptrs:\n");
  for(int i = 0; i < game->n_projectiles; i++){
    if(game->active_projectiles[i] == NULL){
      game_debug("Projectile %d is NULL ptr\n", i);
    }
  }
  game_debug("\nTesting projectile ptr correspondence:\n");
  for(int i = 0; i < game->n_projectiles; i++){
    game_debug("[%d]", game->active_projectiles[i] - my_projectiles);
  }

  game_debug("\nTesting projectile lifetime:\n");
  for(int i = 0; i < game->n_projectiles; i++){
    game_debug("[%d]", game->active_projectiles[i]->lifetime);
  }

  game_debug("\n\n## PROJECTILE STATUS DONE ##:\n");
}
