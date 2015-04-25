#include "logic.h"
#include "pqueue.h"


// Represents collision between an asteroid and a projectile. Uses IDs to
// verify that the asteroid and projectile still exist.
static struct collision
{
  struct asteroid* asteroid;
  int asteroid_id;

  struct projectile* projectile;
  int projectile_id;

  unsigned time;
}


// Collision queue used to keep track of potential collisions. When an asteroid
// or a projectile is spawned, all potential collisions are calculated and
// added to the queue.
static struct pqueue* my_collision_queue;
static struct asteroid*   active_asteroids;
static struct projectile* active_projectiles;

// Used to store collisions.
static struct collision* my_collisions;
int n_collisions;
#define INITIAL_N_COLLISIONS 15 

// Collision comparator. Returns negative, zero, or positive to signify that
// collision1 happens before, at the same time, or after collision2
// respectively.
static inline int compare_collisions(void* collision1, void* collision2)
{
  int time1 = ((struct collision*) collision1)->time;
  int time2 = ((struct collision*) collision2)->time;
  return time1 - time2;
}

void update_projectile_collisions(struct projectile* new_projectile){
  // TODO 
}

void update_asteroid_collisions(struct projectile* new_projectile){
  // TODO 
}

struct collision* get_next_collision(){
  // TODO
  return NULL;
}

// Module init
void init_collisions(struct gamestate* game){
  my_collisions = (struct collision*) malloc(sizeof(struct collision) * INITIAL_N_COLLISIONS);
  n_collisions = 0;
  my_collision_queue = pqueue_new(&compare_collisions);
  active_asteroids = game->active_asteroids;
}

// Module release
void release_collisions(){
  free(my_collisions);
  pqueue_destroy(my_collision_queue);
}
