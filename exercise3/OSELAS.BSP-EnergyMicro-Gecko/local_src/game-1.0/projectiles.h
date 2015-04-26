#include "logic.h"

#define MAX_AMOUNT_PROJECTILES  5
#define PROJECTILE_LIFETIME     20

void do_shoot(void);
void update_projectiles(void);
void kill_projectile_id(int id);
void kill_projectile(int index);
void init_projectiles(gamestate* game_ptr);