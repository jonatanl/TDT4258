#include "logic.h"
#include "pqueue.h"

#define DEFAULT_MAX_TIME 1000

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
static struct gamestate my_gamestate;

// Used to store collisions.
static struct collision* my_collisions;
int n_collisions;
int max_collisions;
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
  struct asteroid** active_asteroids = my_gamestate->active_asteroids;
  int n_asteroids = my_gamestate->n_asteroids;

  struct polygon projectile_poly = new_projectile->poly;
  int32_t projectile_x_speed = new_projectile->x_speed;
  int32_t projectile_y_speed = new_projectile->y_speed;

  struct asteroid asteroid;
  struct polygon asteroid_poly;
  int32_t asteroid_x_speed;
  int32_t asteroid_y_speed;

  const int time_max = DEFAULT_MAX_TIME;
  int time_first;
  int time_last;

  struct collision* new_collision;

  // Add all potential collisions to the collision queue
  for(int i=0; i<n_asteroids; i++){
    asteroid = active_asteroids[i];
    asteroid_poly = asteroid_poly->poly;
    asteroid_x_speed = asteroid->x_speed;
    asteroid_y_speed = asteroid->y_speed;

    // Run the intersection algorithm
    if(get_intersection_time_poly_poly(
          projectile_poly,
          asteroid_poly,
          projectile_x_speed,
          projectile_y_speed,
          asteroid_x_speed,
          asteroid_y_speed,
          time_max,
          &time_first,
          &time_last))
    {
      // If the collision list is full, double its size
      if(n_collisions == max_collisions){
        realloc(my_collisions, sizeof(struct collision) * max_collisions * 2);
      }

      // Initialize the new collision
      new_collision = my_collisions[n_collisions++];
      new_collision->asteroid = asteroid;
      new_collision->asteroid_id = asteroid->asteroid_id;
      new_collision->projectile = projectile;
      new_collision->projectile_id = projectile->projectile_id;
      new_collision->time = time_first;

      // Add the new collision to the collision queue
      pqueue_insert(my_collision_queue, new_collision); 
    }
  }
}

void update_asteroid_collisions(struct asteroid* new_asteroid){
  struct projectile** active_projectiles = my_gamestate->active_projectiles;
  int n_projectiles = my_gamestate->n_projectiles;

  struct polygon asteroid_poly = new_asteroid->poly;
  int32_t asteroid_x_speed = new_asteroid->x_speed;
  int32_t asteroid_y_speed = new_asteroid->y_speed;

  struct projectile projectile;
  struct polygon projectile_poly;
  int32_t projectile_x_speed;
  int32_t projectile_y_speed;

  const int time_max = DEFAULT_MAX_TIME;
  int time_first;
  int time_last;

  struct collision* new_collision;

  // Add all potential collisions to the collision queue
  for(int i=0; i<n_projectiles; i++){
    projectile = active_projectiles[i];
    projectile_poly = projectile_poly->poly;
    projectile_x_speed = projectile->x_speed;
    projectile_y_speed = projectile->y_speed;

    // Run the intersection algorithm
    if(get_intersection_time_poly_poly(
          asteroid_poly,
          projectile_poly,
          asteroid_x_speed,
          asteroid_y_speed,
          projectile_x_speed,
          projectile_y_speed,
          time_max,
          &time_first,
          &time_last))
    {
      // If the collision list is full, double its size
      if(n_collisions == max_collisions){
        realloc(my_collisions, sizeof(struct collision) * max_collisions * 2);
      }

      // Initialize the new collision
      new_collision = my_collisions[n_collisions++];
      new_collision->asteroid = asteroid;
      new_collision->asteroid_id = asteroid->asteroid_id;
      new_collision->projectile = projectile;
      new_collision->projectile_id = projectile->projectile_id;
      new_collision->time = time_first + my_gamestate->time;

      // Add the new collision to the collision queue
      pqueue_insert(my_collision_queue, new_collision); 
    }
  }
}

struct collision* get_next_collision(){
  return pqueue_empty(my_collision_queue) ? NULL : pqueue_poll(my_collision_queue);
}

// Module init
void init_collisions(struct gamestate* gamestate){
  my_collisions = (struct collision*) malloc(sizeof(struct collision) * INITIAL_N_COLLISIONS);
  n_collisions = 0;
  max_collisions = INITIAL_N_COLLISIONS;
  my_collision_queue = pqueue_new(&compare_collisions);
  my_gamestate = gamestate;
}

// Module release
void release_collisions(){
  free(my_collisions);
  pqueue_destroy(my_collision_queue);
}
