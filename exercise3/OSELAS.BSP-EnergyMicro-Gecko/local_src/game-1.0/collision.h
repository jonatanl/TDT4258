// Include guard
#ifndef _COLLISION_H
#define _COLLISION_H

#include "logic.h"

// Represents collision between an asteroid and a projectile. Uses IDs to
// verify that the asteroid and projectile still exist.
struct collision
{
  struct asteroid* asteroid;
  int asteroid_id;

  struct projectile* projectile;
  int projectile_id;

  unsigned time;
};

// Returns a pointer to the next collision, or NULL if there is no collisions
// for the current frame.
struct collision* get_next_collision();

// Updates the module with collisions for a newly created projectile
void update_projectile_collisions(struct projectile* new_projectile);

// Updates the module with collisions for a newly created asteroid
void update_asteroid_collisions(struct projectile* new_projectile);

// Initialize module resources
void init_collisions(struct gamestate* game);

// Release module resources
void release_collisions();

#endif // !_COLLISION_H
