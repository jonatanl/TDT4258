// Include guard
#ifndef _LOGIC_H
#define _LOGIC_H

#define SHOOT         (1 << 0)
#define ROTATE_LEFT   (1 << 1)
#define ACCELERATE    (1 << 2)
#define ROTATE_RIGHT  (1 << 3)
#define MAX_AMOUNT_ASTEROIDS 40

struct vertice{
    uint16_t x_pos;
    uint16_t y_pos;
};

struct polygon{
    uint16_t n_vertices;
    struct vertice* vertices;
};

struct input{
    uint8_t shoot : 1;
    uint8_t turn_left : 1;
    uint8_t accelerate : 1;
    uint8_t turn_right : 1;
};

// Struct for ship properties. Only one(two?) should ever exist.
struct ship_object{
    uint16_t orientation; // Could be float? Currently should be something like 0 - 360
    int16_t x_speed;
    int16_t y_speed;
    uint16_t x_pos;
    uint16_t y_pos;
    struct polygon real_poly;       // The idea is to keep 
    struct polygon current_poly;
};

// Struct for asteroid properties
struct asteroid{
    int16_t x_speed;
    int16_t y_speed;
    uint16_t x_pos;
    uint16_t y_pos;
    struct polygon real_poly;
}; 

// Implementation of the logic module
struct gamestate{
    struct ship_object ship;
    uint16_t n_asteroids;
    struct asteroid** asteroids;
};

typedef struct vertice vertice;
typedef struct ship_object ship_object;
typedef struct gamestate gamestate;
typedef struct polygon polygon;
typedef struct input input;
typedef struct asteroid asteroid;


void do_logic(struct input);
struct ship_object init_ship(void);
struct asteroid* init_asteroid(void);
void rotate_polygon(struct polygon poly, int16_t degrees);
void update_ship(struct ship_object* ship, struct input input);
void init_logic(uint16_t n_asteroids, struct gamestate* game);

#endif // !_LOGIC_H
