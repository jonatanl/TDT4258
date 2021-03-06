
#define LARGE_BRANCH            2
#define MEDIUM_BRANCH           2

#define START_ASTEROIDS         3
#define LARGE_ASTEROIDS         START_ASTEROIDS
#define MEDIUM_ASTEROIDS        START_ASTEROIDS*LARGE_BRANCH
#define SMALL_ASTEROIDS         MEDIUM_ASTEROIDS*MEDIUM_BRANCH

#define MAX_AMOUNT_ASTEROIDS    LARGE_ASTEROIDS + MEDIUM_ASTEROIDS + SMALL_ASTEROIDS  
#define MAX_ALIVE_ASTEROIDS     LARGE_ASTEROIDS*LARGE_BRANCH*MEDIUM_BRANCH

void init_asteroids();
void kill_asteroid(int index);
void kill_asteroid_id(int id);
void update_asteroids();