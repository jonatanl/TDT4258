
// Updates the module with collisions for a newly created projectile
void update_projectile_collisions(struct projectile* new_projectile);

// Updates the module with collisions for a newly created asteroid
void update_asteroid_collisions(struct projectile* new_projectile);

// Returns a pointer to the next collision, or NULL if there is no collisions
// for the current frame.
struct collision* get_next_collision();

// Initialize module resources
void init_collisions(struct gamestate* game);

// Release module resources
void release_collisions();
