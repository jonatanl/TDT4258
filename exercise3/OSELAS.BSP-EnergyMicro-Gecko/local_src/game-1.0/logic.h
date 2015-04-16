// Include guard
#ifndef _LOGIC_H
#define _LOGIC_H

#include <stdint.h>

#define MAX_AMOUNT_ASTEROIDS 40


struct polygon{
    uint16_t n_vertices;
    uint16_t* x_vertices;
    uint16_t* y_vertices;
};

// Struct for ship properties. Only one(two?) should ever exist.
struct ship_object{
    uint16_t orientation; // Could be float? Currently should be something like 0 - 360
    int16_t x_speed;
    int16_t y_speed;
    uint16_t x_pos;
    uint16_t y_pos;
    struct polygon real_poly;
    // struct polygon current_poly;
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
typedef struct asteroid asteroid;


void do_logic(uint8_t input);
void update_gamestate(struct gamestate* game);
struct ship_object init_ship(void);
struct asteroid* init_asteroid(void);
void rotate_polygon(struct polygon poly, int16_t degrees);
void update_ship(struct ship_object* ship, uint8_t input);
void init_logic(uint16_t n_asteroids, struct gamestate* game);
asteroid* make_asteroid(uint16_t n_vertices, uint16_t* x_vertices, uint16_t* y_vertices);

#endif // !_LOGIC_H
